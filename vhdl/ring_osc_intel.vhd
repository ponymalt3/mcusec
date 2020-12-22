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

architecture intel of ring_osc is

  component LCELL
    port(
      a_in : in std_logic; 
      a_out : out std_logic);
  end component;

  signal osc_elements : std_ulogic_vector(NumElements-1 downto 0);

begin  -- architecture intel

  LCELL_init : LCELL
    port map (a_in=>not osc_elements(osc_elements'length-1),a_out=>osc_elements(0));

  gen_ring_osc: for i in 1 to osc_elements'length-1 generate
    LCELL_x : LCELL
      port map (a_in=>not osc_elements(osc_elements'length-1),a_out=>osc_elements(0));
  end generate gen_ring_osc;

  clk_o <= osc_elements(0);

end architecture intel;
