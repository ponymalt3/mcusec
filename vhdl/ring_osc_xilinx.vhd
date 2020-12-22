--  MIT License
--  
--  Copyright (c) 2020 Malte Graeper
--  
--  Permission is hereby granted, free of charge, to any person obtaining a copy of
--  this software and associated documentation files (the "Software"), to deal in
--  the Software without restriction, including without limitation the rights to
--  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
--  the Software, and to permit persons to whom the Software is furnished to do so,
--  subject to the following conditions:
--  - The above copyright notice and this permission notice shall be included in
--    all copies or substantial portions of the Software.
--  - The Software is provided "as is", without warranty of any kind, express or
--    implied, including but not limited to the warranties of merchantability,
--    fitness for a particular purpose and noninfringement. In no event shall the
--    authors or copyright holders be liable for any claim, damages or other
--    liability, whether in an action of contract, tort or otherwise, arising from,
--    out of or in connection with the Software or the use or other dealings in the
--    Software.

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

Library UNISIM;
use UNISIM.vcomponents.all;

architecture xilinx of ring_osc is

  signal osc_elements : std_ulogic_vector(NumElements-1 downto 0);

begin  -- architecture xilinx

  LUT1_init : LUT1
    generic map(INIT=>"01")
    port map(O=>osc_elements(0),I0=>osc_elements(osc_elements'length-1));

  gen_ring_osc: for i in 1 to osc_elements'length-1 generate
    LUT1_x : LUT1
      generic map(INIT=>"10")
      port map(O=>osc_elements(i),I0=>osc_elements(i-1));
  end generate gen_ring_osc;

  clk_o <= osc_elements(0);

end architecture xilinx;
