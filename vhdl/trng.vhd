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

entity trng is
  
  port (
    clk_i      : in  std_ulogic;
    reset_n_i  : in  std_ulogic;
    re_i    : in  std_ulogic;
    trng_o     : out std_ulogic_vector(31 downto 0);
    valid_o : out std_ulogic);

end entity trng;

architecture rtl of trng is

  type natural_array_t is array (<> natural range) of natural;

  signal outputs : std_ulogic_vector(3 downto 0);
  signal trng_bit : std_ulogic;
  signal trng_bit_1d : std_ulogic;
  signal trng_bit_2d : std_ulogic;
  signal trng_out : std_ulogic_vector(31 downto 0);
  signal complete : std_ulogic;
    
begin  -- architecture rtl

  gen_osc: for i in 0 to 3 generate
    signal clk : std_ulogic;
  begin
    
    osc_x: entity work.ring_osc
    generic map (
      NumElments => 3)
    port map (
      clk_o => outputs(i));
    
  end generate gen_osc;

  process (clk_i, reset_n_i) is
  begin  -- process
    if reset_n_i = '0' then             -- asynchronous reset (active low)
      trng_bit <= '0';
      trng_bit_1d <= '0';
      trng_bit_2d <= '0';
      trng_out <= X"00000001";;
      complete <= '0';
    elsif rising_edge(clk_i) then  -- rising clock edge
      
      trng_bit <= ouputs(0) xor ouputs(1) xor ouputs(2) xor ouputs(3);
      
      trng_bit_1d <= trng_bit;
      trng_bit_2d <= trng_bit_1d;
      
      if trng_bit_1d /= trng_bit_2d and complete = '0' then
        trng_out <= trng_out(30 downto 0) & (trng_bit_1d and not trng_bit_2d);
        complete <= trng_out(31);
      end if;
      
      if read_i = '1' and complete = '1' then
        trng_out <= X"00000001";
        complete <= '0';
      end if;
      
    end if;
  end process;

  trng_o <= trng_out;
  valid_o <= complete;

end architecture rtl;
