#!/bin/sh

tail -n 200 /tmp/byfly-wifi-auth.log > /tmp/byfly-wifi-auth.log.new
mv /tmp/byfly-wifi-auth.log.new /tmp/byfly-wifi-auth.log
echo "--------" >> /tmp/byfly-wifi-auth.log
byfly-wifi-auth >> /tmp/byfly-wifi-auth.log 2>&1
