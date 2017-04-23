LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.all;

ENTITY QEI_testbench IS END;

ARCHITECTURE BEHAVIOR OF QEI_testbench IS
	COMPONENT HOLOBOARD IS 
		PORT (
			RESET_I		: IN  	STD_LOGIC;
			SPI_CLK_I   : INOUT	STD_LOGIC;
			SPI_SS_I    : IN 	STD_LOGIC;
			SPI_MOSI_I  : INOUT	STD_LOGIC;
			SPI_MISO_O	: INOUT	STD_LOGIC;
			QE0_CHA_I	: IN 	STD_LOGIC;
			QE0_CHB_I	: IN 	STD_LOGIC;
			QE1_CHA_I	: IN 	STD_LOGIC;
			QE1_CHB_I	: IN 	STD_LOGIC;
			QE2_CHA_I	: IN 	STD_LOGIC;
			QE2_CHB_I	: IN 	STD_LOGIC;
			QE3_CHA_I	: IN 	STD_LOGIC;
			QE3_CHB_I	: IN 	STD_LOGIC;
			HOLD_QEx_I	: IN 	STD_LOGIC
			--PWM0_IN1_o	: OUT	STD_LOGIC;
			--PWM0_IN2_o	: OUT	STD_LOGIC;
			--PWM1_IN1_o	: OUT	STD_LOGIC;
			--PWM1_IN2_o	: OUT	STD_LOGIC;
			--PWM2_IN1_o	: OUT	STD_LOGIC;	
			--PWM2_IN2_o	: OUT	STD_LOGIC;
			--PWM3_IN1_o	: OUT	STD_LOGIC;
			--PWM3_IN2_o	: OUT	STD_LOGIC
			);
	END COMPONENT;
	
	SIGNAL reset, spi_ss : std_logic := '1';
	SIGNAL qe0_a,qe0_b,qe1_a,qe1_b : std_logic := '0';
	SIGNAL spi_clk, spi_mosi, spi_miso : std_logic := '0';
	SIGNAl hold	: std_logic := '0';
	
BEGIN
	-- Instantiate the Unit Under Test (UUT)
	UUT : HOLOBOARD
	PORT MAP ( 	RESET_I => reset,
				SPI_CLK_I => spi_clk,
				SPI_SS_I => spi_ss,
				SPI_MOSI_I => spi_mosi,
				SPI_MISO_O => spi_miso,
				QE0_CHA_I => qe0_a,
				QE0_CHB_I => qe0_b,
				QE1_CHA_I => qe0_b,
				QE1_CHB_I => qe0_a,
				QE2_CHA_I => qe1_a,
				QE2_CHB_I => qe1_b,
				QE3_CHA_I => qe1_b,
				QE3_CHB_I => qe1_a,
				HOLD_QEx_I => hold
				);
				
	-- Generate signal to test counting and decounting with two rates
	qe0_a <= not qe0_a after 100 ns;
	qe0_b <= qe0_a after 50 ns;
  	qe1_a <= not qe1_a after 1 us;
	qe1_b <= qe1_a after 500 ns;

	PROCESS
	BEGIN
		WAIT FOR 1 us;
			reset <= '0';
		WAIT FOR 100 ns;
			spi_ss <= '0';
		WAIT;
	END PROCESS;
	

	WITH spi_ss SELECT 
		spi_clk <= 	not spi_clk after 500 ns when '0',
					'0' WHEN OTHERS;
	
	
	PROCESS(spi_clk)
	VARIABLE index : integer :=47;
	BEGIN
		IF(rising_edge(spi_clk)) THEN
			CASE index IS
				WHEN 41 => 
					spi_mosi <= '1';
					hold <= '1';	
				WHEN 42 => 
					spi_mosi <= '0';	   	 --address 010
					hold <= '0';
				WHEN 19 => 
					spi_mosi <= '0';	   	 
					hold <= '1';
				WHEN 18 => 
					spi_mosi <= '0';	   	 --address 001
					hold <= '0';	
				WHEN OTHERS => 
					spi_mosi <= '0';
					hold <= '0';
			END CASE;
				IF(index>0) THEN
					index:=index-1;
				END IF;
		END IF;
	END PROCESS;	

END BEHAVIOR;