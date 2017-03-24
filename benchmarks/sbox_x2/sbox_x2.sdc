# ####################################################################

#  Created by Genus(TM) Synthesis Solution 16.10-p006_1 on Fri Jan 27 18:30:42 -0800 2017

# ####################################################################

set sdc_version 1.7

set_units -capacitance 1.0fF
set_units -time 1000.0ps

# Set the current design
current_design sbox_x2

create_clock -name "clk" -period 2.0 -waveform {0.0 1.0} 
set_clock_gating_check -setup 0.0 
set_input_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {a_in[0]}]
set_input_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {a_in[1]}]
set_input_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {a_in[2]}]
set_input_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {a_in[3]}]
set_input_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {a_in[4]}]
set_input_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {a_in[5]}]
set_input_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {a_in[6]}]
set_input_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {a_in[7]}]
set_output_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {d_out[0]}]
set_output_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {d_out[1]}]
set_output_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {d_out[2]}]
set_output_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {d_out[3]}]
set_output_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {d_out[4]}]
set_output_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {d_out[5]}]
set_output_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {d_out[6]}]
set_output_delay -clock [get_clocks clk] -add_delay 0.0 [get_ports {d_out[7]}]
set_wire_load_mode "enclosed"
set_dont_use [get_lib_cells aes_sbox/aes_sbox]
