#!/bin/sh

iptables -P OUTPUT DROP
iptables -P INPUT ACCEPT
iptables -I OUTPUT -p icmp -j ACCEPT
iptables -I INPUT -p tcp --dport 22 -j DROP

