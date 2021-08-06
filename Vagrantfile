# -*- mode: ruby -*-
# vi: set ft=ruby :
Vagrant.configure("2") do |config|
  config.vm.provision "shell", inline: "echo Hello"


  config.ssh.username = 'root'
  config.ssh.password = 'vagrant'
  config.ssh.insert_key = 'true'
  config.ssh.extra_args = [ '-o', 'StrictHostKeyChecking=no',
                            '-o', 'UserKnownHostsFile=/dev/null' ]
  
  config.vbguest.auto_update = false
  config.vbguest.no_install = false

  # Create development box  
  config.vm.define "devel" do |devel|
    devel.vm.box = "ChrisBrumgard/kernel_devel"

    # Set the hostname
    devel.vm.hostname = "devel"

    # Private network between VMs
    devel.vm.network :private_network, ip: "10.0.3.2"

    # Fixed SSH port 
    devel.vm.network "forwarded_port", id: "ssh", host: 22000, guest: 22, auto_correct: false

  end

  # Create the testbox
  config.vm.define "testbox" do |testbox|
    testbox.vm.box = "ChrisBrumgard/kernel_devel"
    
    # Set the hostname
    testbox.vm.hostname = "testbox"

    # Private network between VMs
    testbox.vm.network :private_network, ip: "10.0.3.3"

    testbox.vm.provider "virtualbox" do |vb|
      # Enable the serial port
      vb.customize ["modifyvm", :id, "--uart1", "0x3f8", "4"]

      # Serve the serial port connection over tcp at port 1234
      vb.customize ["modifyvm", :id, "--uartmode1", "tcpserver", "1234"]
    end
 end
end
