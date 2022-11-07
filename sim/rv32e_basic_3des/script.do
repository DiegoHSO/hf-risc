# Trabalho Final - Sistemas Embarcados
# Script
# Diego H. S. Oliveira e Lucas H. Hassmann
# diego.h@edu.pucrs.br, lucas.hassmann@edu.pucrs.br

vlib work
  
vcom ../../riscv/core_rv32e/bshifter.vhd 
vcom ../../riscv/core_rv32e/alu.vhd 
vcom ../../riscv/core_rv32e/reg_bank.vhd 
vcom ../../riscv/core_rv32e/control.vhd 
vcom ../../riscv/core_rv32e/datapath.vhd 
vcom ../../riscv/core_rv32e/int_control.vhd 
vcom ../../riscv/core_rv32e/cpu.vhd 
vcom ../../devices/controllers/uart/uart.vhd
vcom ../../devices/peripherals/basic_soc.vhd 
vcom ../../riscv/sim/boot_ram.vhd 
vcom ../../riscv/sim/ram.vhd 
vcom ../../devices/controllers/3des/tdes_top.vhd 
vcom ../../devices/controllers/3des/add_key.vhd 
vcom ../../devices/controllers/3des/add_left.vhd 
vcom ../../devices/controllers/3des/block_top.vhd 
vcom ../../devices/controllers/3des/des_cipher_top.vhd 
vcom ../../devices/controllers/3des/des_top.vhd 
vcom ../../devices/controllers/3des/e_expansion_function.vhd 
vcom ../../devices/controllers/3des/key_schedule.vhd 
vcom ../../devices/controllers/3des/p_box.vhd 
vcom ../../devices/controllers/3des/s_box.vhd 
vcom ../../devices/controllers/3des/s1_box.vhd 
vcom ../../devices/controllers/3des/s2_box.vhd 
vcom ../../devices/controllers/3des/s3_box.vhd 
vcom ../../devices/controllers/3des/s4_box.vhd 
vcom ../../devices/controllers/3des/s5_box.vhd 
vcom ../../devices/controllers/3des/s6_box.vhd 
vcom ../../devices/controllers/3des/s7_box.vhd 
vcom ../../devices/controllers/3des/s8_box.vhd 
vcom ../../riscv/sim/hf-riscv_basic_soc_3des_tb.vhd

vsim -voptargs="+acc" tb

set StdArithNoWarnings 1
set StdVitalGlitchNoWarnings 1

add wave sim:/*
# do wave.do

run 10 ms