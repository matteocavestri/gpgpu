# Rocky Linux AMD GPU configuration

In this guide, we will address the installation of ROCm on the Rocky Linux operating system (RHEL).

### Install Epel

On Rocky Linux 9.3

```bash
sudo dnf upgrade --refresh
sudo dnf config-manager --set-enabled crb
sudo dnf install \
    https://dl.fedoraproject.org/pub/epel/epel-release-latest-9.noarch.rpm \
    https://dl.fedoraproject.org/pub/epel/epel-next-release-latest-9.noarch.rpm
```

### Install ROCm

This installs version 6.0.2 of ROCm, if you want a different version visit [AMD Index](https://repo.radeon.com/amdgpu-install)

```bash
sudo yum install kernel-headers kernel-devel
sudo usermod -a -G render,video $LOGNAME
sudo yum install https://repo.radeon.com/amdgpu-install/6.0.2/rhel/9.3/amdgpu-install-6.0.60002-1.el9.noarch.rpm
# sudo yum install https://repo.radeon.com/amdgpu-install/5.4.6/rhel/9.2/amdgpu-install-5.4.50406-1.el9.noarch.rpm RX500 series last OpenCL support
sudo yum clean all
sudo yum install amdgpu-dkms
sudo yum install rocm
```
