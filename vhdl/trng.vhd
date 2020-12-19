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
    clk_i     : in  std_ulogic;
    reset_n_i : in  std_ulogic;
    trng_o    : out std_ulogic_vector(31 downto 0));

end entity trng;

architecture rtl of trng is

  function mix (
    signal data : unsigned(15 downto 0);
    constant even : boolean)
    return unsigned(15 downto 0) is
    variable result : unsigned(15 downto 0);
  begin
    for i in 0 to 7 loop
      if even then
        result(2*i+0) := data(30-2*i);
        result(2*i+1) := data(2*i+1);
      else
        result(2*i+0) := data(2*i+0);
        result(2*i+1) := data(31-2*i);
      end if;
    end loop;  -- i

    return result;
  end;

  type natural_array_t is array (<> natural range) of natural;
  type u16_array_t is array (<> natural range) of unsigned(15 downto 0);

  constant delay_config : natural_array_t_t(3 downto 0) := (19,13,7,5);
  signal counters : u16_array_t(3 downto 0);    
    
begin  -- architecture rtl

  gen_osc: for i in 0 to 3 generate
    signal clk : std_ulogic;
  begin
    
    osc_x: entity work.ring_osc
    generic map (
      NumElments => delay_config(i))
    port map (
      clk_o => clk);

    process (clk, reset_n_i) is
    begin  -- process
      if reset_n_i = '0' then             -- asynchronous reset (active low)
        counters(i) <= to_unsigned(0,16);
      elsif rising_edge(clk) then  -- rising clock edge
        counters(i) <= counters(i)+1;
      end if;
    end process;
    
  end generate gen_osc;

  process (clk_i, reset_n_i) is
  begin  -- process
    if reset_n_i = '0' then             -- asynchronous reset (active low)
      trng_1d <= (others => '0');
      trng_2d <= (others => '0');
    elsif rising_edge(clk_i) then  -- rising clock edge
      trng_1d <= ((X"0000" & mix(counters(0),true))*(X"0000" & mix(counters(1),false)))*
                 ((X"0000" & mix(counters(2),false))*(X"0000" & mix(counters(3),true)));
      trng_2d <= trng_1d;      
    end if;
  end process;

  trng_o <= trng_2d;

end architecture rtl;
