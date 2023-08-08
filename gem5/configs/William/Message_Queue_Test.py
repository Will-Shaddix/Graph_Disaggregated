# -*- coding: utf-8 -*-
# Copyright (c) 2017 Jason Lowe-Power
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

""" This file creates a barebones system and executes 'hello', a simple Hello
World application. Adds a simple memobj between the CPU and the membus.

This config file assumes that the x86 ISA was built.
"""

# import the m5 (gem5) library created when gem5 is built
import m5
# import all of the SimObjects
from m5.objects import *
from caches import *

# import the SimpleOpts module
from common import SimpleOpts
import math
# create the system we are going to simulate
system = System()

# Set the clock fequency of the system (and all of its children)
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()

# Set up the system
system.mem_mode = 'timing'               # Use timing accesses
system.mem_ranges = [AddrRange('4KiB', '512MB')] # Create an address range
addr_range = system.mem_ranges[0]
# Create a simple CPU
system.cpu = TimingSimpleCPU()


# Create a memory bus, a coherent crossbar, in this case
system.membus = SystemXBar(width = 64, max_routing_table_size = 16777216)
system.queue_dcache_xbar = IOXBar(width = 64)
system.queue_xbar = IOXBar(width = 64)

system.cpu.icache = L1ICache()
system.cpu.dcache = L1DCache()

# hook the CPU ports up to the membus
system.cpu.icache_port = system.cpu.icache.cpu_side
system.cpu.dcache_port = system.queue_dcache_xbar.cpu_side_ports

system.cpu.icache.mem_side = system.membus.cpu_side_ports

system.queue_dcache_xbar.mem_side_ports = system.cpu.dcache.cpu_side # system.queue_dcache_xbar.mem_side_ports = system
system.cpu.dcache.mem_side = system.membus.cpu_side_ports

queue_ranges = [AddrRange('0', '4KiB')] # address range for queue

system.bridge = Bridge(ranges=queue_ranges)

system.bridge.cpu_side_port = system.queue_dcache_xbar.mem_side_ports
system.bridge.mem_side_port = system.queue_xbar.cpu_side_ports

system.msg_queue = MessageQueue(my_range=queue_ranges[0])

system.msg_queue.cpu_side = system.queue_xbar.mem_side_ports



# create the interrupt controller for the CPU and connect to the membus
system.cpu.createInterruptController()
system.cpu.interrupts[0].pio = system.membus.mem_side_ports
system.cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
system.cpu.interrupts[0].int_responder = system.membus.mem_side_ports

mem_ctrls = []

num_chnls = 4
intlv_bits = int(math.log(num_chnls, 2))
cache_line_size = 64
intlv_low_bit = int(math.log(cache_line_size, 2))

for chnl in range(num_chnls):
            interface = HBM_1000_4H_1x128()
            interface.range = AddrRange(addr_range.start, size = addr_range.size(),
                        intlvHighBit = intlv_low_bit + intlv_bits - 1,
                        xorHighBit = 0,
                        intlvBits = intlv_bits,
                        intlvMatch = chnl)
            ctrl = MemCtrl()
            ctrl.dram = interface

            #ctrl.dram.null = True
            #ctrl.dram.addr_mapping = addr_map
            #ctrl.dram.page_policy = page_policy
            mem_ctrls.append(ctrl)

system.mem_ctrls = mem_ctrls

for mem_ctrl in system.mem_ctrls:
    mem_ctrl.port = system.membus.mem_side_ports

# Create a DDR3 memory controller and connect it to the membus
# system.mem_ctrl = MemCtrl()
# # system.mem_ctrl.dram = DDR3_1600_8x8()
# # system.mem_ctrl.dram.range = system.mem_ranges[0]
# system.mem_ctrl.dram = HBM_1000_4H_1x128()
# system.mem_ctrl.dram.range = system.mem_ranges[0]
# system.mem_ctrl.port = system.membus.mem_side_ports

# # Connect the system up to the membus
system.system_port = system.membus.cpu_side_ports

# Create a process for a simple "Hello World" application
process = Process()
# Set the command
# grab the specific path to the binary
thispath = os.path.dirname(os.path.realpath(__file__))
binpath = os.path.join(thispath, '../../',
                       'tests/test-progs/hello/bin/x86/linux/hello')
# cmd is a list which begins with the executable (like argv)
process.cmd = [binpath]
# Set the cpu to use the process as its workload and create thread contexts
system.cpu.workload = process
system.cpu.createThreads()

system.workload = SEWorkload.init_compatible(binpath)

# set up the root SimObject and start the simulation
root = Root(full_system = False, system = system)
# instantiate all of the objects we've created above
m5.instantiate()

print("Beginning simulation!")
exit_event = m5.simulate()
print('Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause()))
