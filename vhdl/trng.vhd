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
    re_i       : in  std_ulogic;
    trng_o     : out std_ulogic_vector(3 downto 0);
    valid_o    : out std_ulogic);

end entity trng;

architecture rtl of trng is

  type natural_array_t is array (natural range <>) of natural;
  constant delay_config : natural_array_t(3 downto 0) := (7,11,17,29);
  
  function mix (
    signal data : unsigned;
    constant even : boolean)
    return unsigned is
    variable result : unsigned(data'length-1 downto 0);
  begin
    for i in 0 to data'length/2-1 loop
      if even then
        result(2*i+0) := data(data'length-2-2*i);
        result(2*i+1) := data(2*i+1);
      else
        result(2*i+0) := data(2*i+0);
        result(2*i+1) := data(data'length-1-2*i);
      end if;
    end loop;  -- i

    return result;
  end;

  type u8_array_t is array (natural range <>) of unsigned(7 downto 0);  
  signal counters : u8_array_t(3 downto 0);

  type state_t is (ST_START,ST_WAIT,ST_COMPLETE);
  signal state : state_t;

  signal trng : unsigned(31 downto 0);
  signal trng_1d : std_ulogic_vector(31 downto 0);
  signal trng_out : std_ulogic_vector(3 downto 0);
  signal trng_compare : std_ulogic_vector(3 downto 0);
  signal trng_bits_changed : std_ulogic_vector(3 downto 0);
    
begin  -- architecture rtl

  gen_osc: for i in 0 to 3 generate
    signal clk : std_ulogic;
  begin    
    osc_x: entity work.ring_osc
      generic map (
        NumElements => delay_config(i))
      port map (
        clk_o => clk);

    process (clk, reset_n_i) is
    begin  -- process
      if reset_n_i = '0' then             -- asynchronous reset (active low)
        counters(i) <= to_unsigned(0,8);
      elsif rising_edge(clk) then  -- rising clock edge
        counters(i) <= counters(i)+1;
      end if;
    end process;    
  end generate gen_osc;

  process (clk_i, reset_n_i) is
  begin  -- process
    if reset_n_i = '0' then             -- asynchronous reset (active low)
      trng <= to_unsigned(0,32);
      trng_1d <= (others => '0');
    elsif rising_edge(clk_i) then  -- rising clock edge
      trng <= (mix(counters(0),true)*mix(counters(1),false))*(mix(counters(2),false)*mix(counters(3),true));
      trng_1d <= trng_1d xor to_stdULogicVector(std_logic_vector(trng));
      trng_out <= trng_1d(31 downto 28) xor trng_1d(27 downto 24) xor
                  trng_1d(23 downto 20) xor trng_1d(19 downto 16) xor
                  trng_1d(15 downto 12) xor trng_1d(11 downto 8) xor
                  trng_1d(7 downto 4) xor trng_1d(3 downto 0);
    end if;
  end process;

  process (clk_i, reset_n_i) is
  begin  -- process
    if reset_n_i = '0' then             -- asynchronous reset (active low)
      trng_compare <= (others => '0');
      trng_o <= (others => '0');
      state <= ST_START;
      trng_bits_changed <= (others => '0');
    elsif rising_edge(clk_i) then  -- rising clock edge
      case state is
        when ST_START =>
          trng_compare <= trng_out;
          trng_bits_changed <= (others => '1');
          state <= ST_WAIT;
        when ST_WAIT =>
          trng_bits_changed <= trng_bits_changed and not (trng_compare xor trng_out);
          if trng_bits_changed = "0000" then
            trng_o <= trng_out;
            state <= ST_COMPLETE;
          end if;
        when ST_COMPLETE =>
          if re_i = '1' then
            state <= ST_START;
          end if;
        when others => null;
      end case;
    end if;
  end process;

  valid_o <= '1' when state = ST_COMPLETE else '0';

end architecture rtl;
