#!/bin/bash -e

trap "vagrant destroy -f; rm -rf /tmp/vbox" EXIT

mkdir -p /tmp/vbox
cp kernel_config /tmp/vbox
cd /tmp/vbox

vagrant plugin update vagrant-disksize
vagrant plugin update vagrant-vbguest

cat <<EOF >Vagrantfile
Vagrant.configure("2") do |config|
  config.vm.box = "centos/8"
  config.vm.provider "virtualbox" do |vb|
      vb.memory = "4096"
      vb.cpus = "8"
      config.disksize.size = "40GB"
      config.vm.synced_folder ".", "/vagrant", disabled: true
      config.vbguest.auto_update = false
  end
end
EOF

vagrant box update
vagrant up
vagrant ssh -c 'sudo parted /dev/sda ---pretend-input-tty <<EOF 
resizepart 
1
Yes
30000
quit
EOF'

vagrant ssh -c 'sudo  xfs_growfs /;
                sudo df -lh;
                sudo dnf update -y
                sudo dnf install -y gcc-toolset-10-toolchain;
                sudo dnf install -y vim cmake zlib-devel libuuid-devel \
                        libblkid-devel openssl-devel elfutils-libelf-devel \
                        libtirpc-devel bc kernel-devel bison flex;
                sudo yum clean all;
                sudo sed  -i "s/[# ]*PermitRootLogin no/PermitRootLogin yes/g" /etc/ssh/sshd_config;
                sudo sed  -i "s/PasswordAuthentication no/PasswordAuthentication yes/g" /etc/ssh/sshd_config;
                sudo bash -c "echo \"PermitRootLogin yes\" >> /etc/ssh/sshd_config";
                sudo bash -c "echo \"PermitEmptyPasswords yes\" >> /etc/ssh/sshd_config";'

vagrant halt

sed -I ''  's/disabled: true/disabled: false/g' Vagrantfile
sed -I '' 's/config.vbguest.auto_update = false/config.vbguest.auto_update = true/g' Vagrantfile

vagrant up

LINUX_VERSION="linux-4.18.20"

vagrant ssh -c "cd /usr/src/kernels; 
                sudo curl -O -L https://mirrors.edge.kernel.org/pub/linux/kernel/v4.x/${LINUX_VERSION}.tar.xz; 
                sudo tar xfJ ${LINUX_VERSION}.tar.xz; 
                cd ${LINUX_VERSION}/; 
                sudo make mrproper; 
                sudo cp /vagrant/kernel_config ./.config"


vagrant ssh -c "cd /usr/src/kernels/${LINUX_VERSION};
                sudo make -j8; 
                sudo make -j8  modules_install; 
                sudo rm -rf /etc/dracut.conf.d/hyperv-drivers.conf;
                sudo make install;
                sudo rm -rf /usr/src/kernels/linux-4.18.20.tar.xz;"

vagrant ssh -c 'sudo bash -c "echo \"source /opt/rh/gcc-toolset-10/enable\" >> /etc/profile.d/bash.sh"'

vagrant ssh -c 'sudo sed -i "s/\(GRUB_CMDLINE_LINUX=\".*\)\"/\1 kgdboc=ttyS0,115200 kdb.cmd_enable=1 nokaslr kmemleak=on\"/g" /etc/default/grub;
                sudo grub2-mkconfig -o /boot/grub2/grub.cfg;
                sudo dnf remove -y --oldinstallonly --setopt installonly_limit=2 kernel
                sudo dd if=/dev/zero of=/EMPTY bs=1M;
                sudo rm -f /EMPTY;
                sudo passwd -d root;
                cat /dev/null > ~/.bash_history && history -c;'

vagrant halt
vagrant up

vagrant ssh -c 'sudo /sbin/rcvboxadd quicksetup $(uname -r)'

vagrant package --output kernel_devel.box
vagrant box add --force kernel_devel kernel_devel.box
vagrant destroy -f
