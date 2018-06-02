#!/bin/sh

iptables -t filter -F
iptables -t filter -A INPUT -p tcp --dport 22 -j ACCEPT
iptables -A OUTPUT -p icmp -j DROP
iptables -P INPUT DROP
iptables -P OUTPUT ACCEPT
iptables -P FORWARD DROP
