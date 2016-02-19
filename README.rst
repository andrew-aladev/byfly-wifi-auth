Byfly Wi-Fi authentication
==========================

There is a bad state ISP "Byfly" in Belarus.
It is a part of outdated state ISP monopoly "Beltelecom".
Many belarusians have no choice and have to use it.

There is a byfly Wi-Fi network.
It does not support any standart authentication methods, it has no encryption.

You can connect to any access point, open web browser, try to view any website and you will be redirected to "https://homewifi.beltelecom.by/login" using "DNS rebind attack".
You can fill login form and after some random time upstream connection will become up. "Success" messages from web login page mean that remote server has accepted your credentials.
You will not receive any message when upstream connection will become up, you have to ping any host.


Warning
-------
You can reconnect to current access point and you will continue using current session.
So any thing can connect to current access point using your MAC address after your disconnect and it will use your session.

* Use only unlimited tariff.
* Do not send unencrypted traffic over this network.
* Do not trust ISP DNS servers.
* Do not trust `<src/etc/ca-bundle.crt>`_ in this repository. Please download such bundle from trusted location and replace it before building.


Goal
----
Provide small binary for openwrt-powered router with 4mb ROM. You can use it on any unix-like systems.


Usage scheme
------------

1. wpa_supplicant will connect to access point with key_mgmt=NONE.

2. dnsmasq (with disabled DNS rebind protection) will receive ip address and nameservers from remote server.

3. dnsmasq will store nameservers in /tmp/resolv.conf.auto, but will provide only trusted public nameserves for both local and remote system.

4. auth application will read /tmp/resolv.conf.auto and make custom dns query to find ip address for "homewifi.beltelecom.by".

5. libcurl will validate "homewifi.beltelecom.by" cert using custom "ca-bundle" and send login queries. It will look like regular web browser.


Build
-----

    ::

     mkdir build && cd build
     cmake .. -DCMAKE_BUILD_TYPE=RELEASE
     make VERBOSE=1 && make package
     ls byfly-wifi-auth-* | xargs -n1 xdg-open


Example of build for openwrt
----------------------------

     ::

      mkdir build && cd build
      wget "https://downloads.openwrt.org/chaos_calmer/15.05/ar71xx/generic/OpenWrt-SDK-15.05-ar71xx-generic_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64.tar.bz2"
      tar xvjf OpenWrt-SDK-15.05-ar71xx-generic_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64.tar.bz2
      export STAGING_DIR="OpenWrt-SDK-15.05-ar71xx-generic_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/"
      cmake .. \
          -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/gcc/openwrt-15.05-ar71xx.cmake \
          -DSTAGING_DIR=$(realpath "$STAGING_DIR") \
          -DCMAKE_BUILD_TYPE=RELEASE_EMBED
      make VERBOSE=1


Example of installation for openwrt
-----------------------------------
I have an old TL-WA901ND v2 router.

    ::

     wget "https://downloads.openwrt.org/chaos_calmer/15.05/ar71xx/generic/OpenWrt-ImageBuilder-15.05-ar71xx-generic.Linux-x86_64.tar.bz2"
     tar xvjf OpenWrt-ImageBuilder-15.05-ar71xx-generic.Linux-x86_64.tar.bz2
     make image PROFILE="TLWA901" PACKAGES=" \
         -opkg \
         -ppp \
         -ppp-mod-pppoe \
         sudo \
         openvpn-polarssl \
         nano \
         libcurl \
     "

to ``/etc/config``:

    ::

     config wifi-iface
         option device   radio0
         option network  wan
         option mode     sta
         option key_mgmt NONE

        option ssid  'byfly WIFI'
        option bssid '<insert bssid>'

     config dnsmasq
         option domainneeded      '1'
         option boguspriv         '1'
         option filterwin2k       '0'
         option localise_queries  '1'
         option rebind_protection '0'
         option rebind_localhost  '1'
         option local             '/lan/'
         option domain            'lan'
         option expandhosts       '1'
         option nonegcache        '0'
         option authoritative     '1'
         option readethers        '1'
         option leasefile         '/tmp/dhcp.leases'
         option resolvfile        '/tmp/resolv.conf.auto'
         option localservice      '1'

     config dhcp 'lan'
         option interface     'lan'
         option start         '100'
         option limit         '150'
         option leasetime     '12h'
         list   'dhcp_option' '6,8.8.8.8,8.8.4.4,208.67.222.222,208.67.220.220'

to ``/etc/init.d/dnsmasq``:

    ::

         #DNS_SERVERS="$DNS_SERVERS 127.0.0.1"
         DNS_SERVERS="8.8.8.8 8.8.4.4 208.67.222.222 208.67.220.220"
         for DNS_SERVER in $DNS_SERVERS ; do
             echo "nameserver $DNS_SERVER" >> /tmp/resolv.conf
         done
     }

     reload_service() {

to ``/etc/sysupgrade.conf``:

    ::

     /etc/sysupgrade.conf
     /etc/config
     /etc/profile
     /etc/firewall.user
     /etc/openvpn
     /etc/shadow
     /etc/sudoers

     /root
     /home/username

     # https://dev.openwrt.org/ticket/19621
     /etc/ssl/certs/

     /etc/init.d/dnsmasq

     /usr/bin/byfly-wifi-auth
     /usr/bin/byfly-wifi-auth.sh
     /etc/byfly-wifi/
     /etc/hotplug.d/iface/99-byfly-wifi-auth

    ::

     mkdir /etc/ssl/certs/

to ``crontab``:

    ::

     */20 * * * * byfly-wifi-auth.sh

Don't forget to enable autostart of crontab.

Let ``192.168.1.1`` be a local ip address of router.

    ::

     scp ./src/byfly-wifi-auth 192.168.1.1:/tmp/
     scp ../src/bin/byfly-wifi-auth.sh 192.168.1.1:/tmp/
     scp ../src/etc/ca-bundle.crt 192.168.1.1:/tmp/
     scp ../src/etc/99-byfly-wifi-auth 192.168.1.1:/tmp/
     
     ssh 192.168.1.1
     
     sudo chown root:root /tmp/byfly-wifi-auth && sudo chmod 0500 /tmp/byfly-wifi-auth
     sudo mv /tmp/byfly-wifi-auth /usr/bin/
     
     sudo chown root:root /tmp/byfly-wifi-auth.sh && sudo chmod 0500 /tmp/byfly-wifi-auth.sh
     sudo mv /tmp/byfly-wifi-auth.sh /usr/bin/
     
     sudo chown root:root /tmp/ca-bundle.crt && sudo chmod 0400 /tmp/ca-bundle.crt
     sudo mkdir /etc/byfly-wifi
     sudo mv /tmp/ca-bundle.crt /etc/byfly-wifi/
     
     sudo touch /etc/byfly-wifi/passwd
     echo -e "123@beltel.by\npassword" | sudo tee /etc/byfly-wifi/passwd
     sudo chown root:root /etc/byfly-wifi/passwd && sudo chmod 0400 /etc/byfly-wifi/passwd
     
     sudo chown root:root /tmp/99-byfly-wifi-auth && sudo chmod 0644 /tmp/99-byfly-wifi-auth
     sudo mv /tmp/99-byfly-wifi-auth /etc/hotplug.d/iface/


LOGS
----
See ``/tmp/byfly-wifi-auth.log``


LICENSE
-------
MIT
