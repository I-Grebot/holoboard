LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.std_logic_unsigned.all;

ENTITY PWM_testbench IS END;

ARCHITECTURE BEHAVIOR OF PWM_testbench IS
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
			HOLD_QEx_I	: IN 	STD_LOGIC;
			PWM0_IN1_O	: OUT	STD_LOGIC;
			PWM0_IN2_O	: OUT	STD_LOGIC;
			PWM1_IN1_O	: OUT	STD_LOGIC;
			PWM1_IN2_O	: OUT	STD_LOGIC;
			PWM2_IN1_O	: OUT	STD_LOGIC;	
			PWM2_IN2_O	: OUT	STD_LOGIC;
			PWM3_IN1_O	: OUT	STD_LOGIC;
			PWM3_IN2_O	: OUT	STD_LOGIC
			);
	END COMPONENT;
	
	SIGNAL reset, spi_ss : std_logic := '1';
	SIGNAL qe0_a,qe0_b,qe1_a,qe1_b : std_logic := '0';
	SIGNAL spi_clk, spi_mosi, spi_miso : std_logic := '1';
	SIGNAl hold	: std_logic := '0';
	SIGNAl pwm0, pwm1, pwm2, pwm3 : std_logic := '0';
	
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
				HOLD_QEx_I => hold,
				PWM0_IN1_O => pwm0,
 				PWM1_IN1_O => pwm1,
 				PWM2_IN1_O => pwm2,	
 				PWM3_IN1_O => pwm3);
				
	PROCESS
	BEGIN
		WAIT FOR 1 us;
			reset <= '0';
		WAIT FOR 100 ns;
			spi_ss <= '0';
		WAIT for 40us;
			spi_ss <= '1';
		WAIT;
	END PROCESS;
	

	WITH spi_ss SELECT 
		spi_clk <= 	not spi_clk after 500 ns when '0',
					'0' WHEN OTHERS;
	
	
	PROCESS(spi_clk)
	VARIABLE index : integer :=31;
	BEGIN
		IF(rising_edge(spi_clk)) THEN
			CASE index IS
				WHEN 31 => 
					spi_mosi <= '1';
				WHEN 26 => 
					spi_mosi <= '1';
					hold <= '1';	
				WHEN 24 => spi_mosi <= '1';	   	 --address 0101
				WHEN 10 => spi_mosi <= '1';
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