#!/usr/bin/python

from mininet.node import OVSBridge
from mininet.net import Mininet
from mininet.topo import Topo
from mininet.link import TCLink
from mininet.cli import CLI

nworkers = 2

class MyTopo(Topo):
    def build(self):
        # set up two hosts
        h1 = self.addHost('h1', ip='10.0.20.2', mac="00:00:00:00:00:08")
        h2 = self.addHost('h2', ip='10.0.20.3', mac="00:00:00:00:00:09")
        
        # create link between two hosts
        self.addLink(h1, h2)

if __name__ == "__main__":

    net = Mininet()
    h1 = net.addHost('h1', ip='10.0.20.2', mac="00:00:00:00:00:08")
    h2 = net.addHost('h2', ip='10.0.20.3', mac="00:00:00:00:00:09")
    net.addLink(h1, h2)

    net.start()
    CLI(net)
    net.stop()