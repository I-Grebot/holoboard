LIBRARY  ieee; 
	USE ieee.std_logic_1164.all; 
	USE ieee.numeric_std.all; 
LIBRARY lattice;
	USE lattice.all;
LIBRARY machxo2;
	USE machxo2.all;
  
ENTITY HOLOBOARD IS 
	PORT (
		--RESET_I		: IN  	STD_LOGIC;
		--SPI_CLK_I   : IN	STD_LOGIC;
		--SPI_SS_I    : IN 	STD_LOGIC;
		--SPI_MOSI_I  : IN	STD_LOGIC;
		--SPI_MISO_O	: OUT	STD_LOGIC;
		--QE0_CHA_I	: IN 	STD_LOGIC;
		--QE0_CHB_I	: IN 	STD_LOGIC;
		--QE1_CHA_I	: IN 	STD_LOGIC;
		--QE1_CHB_I	: IN 	STD_LOGIC;
		--QE2_CHA_I	: IN 	STD_LOGIC;
		--QE2_CHB_I	: IN 	STD_LOGIC;
		--QE3_CHA_I	: IN 	STD_LOGIC;
		--QE3_CHB_I	: IN 	STD_LOGIC;
		PWM0_IN1_O	: OUT	STD_LOGIC;
		PWM0_IN2_O	: OUT	STD_LOGIC;
		PWM1_IN1_O	: OUT	STD_LOGIC;
		PWM1_IN2_O	: OUT	STD_LOGIC;
		PWM2_IN1_O	: OUT	STD_LOGIC;
		PWM2_IN2_O	: OUT	STD_LOGIC;
		PWM3_IN1_O	: OUT	STD_LOGIC;
		PWM3_IN2_O	: OUT	STD_LOGIC
		);
END HOLOBOARD;

ARCHITECTURE BEHAVIOR OF HOLOBOARD IS
	--TYPE STATE_TYPE is (waiting, address, rx_data,prepare_tx,end_of_transmission);
	
	--SIGNAL clk  : STD_LOGIC;																		-- generale clock = 133 Mhz
   	--SIGNAL qei_counter0, qei_counter1, qei_counter2, qei_counter3 : STD_LOGIC_VECTOR(15 DOWNTO 0);	-- qei_counters
	--SIGNAL pwm0_mem, pwm1_mem, pwm2_mem, pwm3_mem : STD_LOGIC_VECTOR(15 DOWNTO 0);					-- pwm mem (signed)
	--SIGNAL pwm0_value, pwm1_value, pwm2_value, pwm3_value : STD_LOGIC_VECTOR(15 DOWNTO 0);			-- pwm input for pwm generator
	--SIGNAL pwm0_sens, pwm1_sens, pwm2_sens, pwm3_sens : STD_LOGIC;									-- 0 = Forward ; 1 = Reverse
	--SIGNAL pwm0, pwm1, pwm2, pwm3 : STD_LOGIC;														-- pwm output from pwm generator
	--SIGNAL machine_state, next_state : STATE_TYPE;													-- state machine to handle SPI communication
	--SIGNAL memory_address : STD_LOGIC_VECTOR(2 DOWNTO 0);
	--SIGNAL memory_rw : STD_LOGIC;																	-- 0 = read ; 1 = write
	--SIGNAL memory_en : STD_LOGIC;																	-- latch to write memory
	--SIGNAL to_memory	: std_logic_vector(15 downto 0);											-- data written in memory
	--SIGNAL from_memory	: std_logic_vector(15 downto 0);											-- data read from memory
	--SIGNAL to_spi	: std_logic_vector(15 downto 0);												-- data to send with SPI
	--SIGNAL from_spi	: std_logic_vector(15 downto 0);												-- data received from SPI
	--SIGNAL spi_irq, tx_load : STD_LOGIC;
	
   
	--COMPONENT OSCH																					-- : internal oscillator				
		--GENERIC(
			--NOM_FREQ: string := "53.20");
		--PORT(
            --STDBY    : IN  STD_LOGIC;
            --OSC      : OUT STD_LOGIC;
            --SEDSTDBY : OUT STD_LOGIC);
	--END COMPONENT;
   
	--COMPONENT SIMPLE_SPI IS 																		-- : SPI instanciation
		--PORT (		
			--RESET_I			:	IN	STD_LOGIC;
			--CLK_I     		:	IN	STD_LOGIC;
			--SCLK_I   		:	IN	STD_LOGIC;
			--SS_I    		:	IN	STD_LOGIC;
			--MOSI_I  		:	IN	STD_LOGIC;
			--MISO_O			:	OUT	STD_LOGIC;
			--DATA_TO_TX_O  	: 	IN 	STD_LOGIC_VECTOR(15 DOWNTO 0);
			--TX_LOAD_I		:	IN	STD_LOGIC;
			--RX_DATA_O	 	: 	OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
			--IRQ_O			:	OUT	STD_LOGIC
		--);
	--END COMPONENT;
   
   	--COMPONENT QEI 																					-- : QEI instanciation
		--PORT (
			--RESET_I			: IN 	STD_LOGIC;
			--CLK_I      		: IN  	STD_LOGIC;
			--QE_CHA_I		: IN 	STD_LOGIC;
			--QE_CHB_I		: IN 	STD_LOGIC;
			--HOLD_I			: IN 	STD_LOGIC;
			--QE_COUNTER_O	: OUT	STD_LOGIC_VECTOR(15 DOWNTO 0));  
   --END COMPONENT;
   
	--COMPONENT MEMORY_MANAGER IS 																	-- : memory manager instanciation
	--PORT (
		--RESET_I			: IN  	STD_LOGIC;
		--CLK_I      		: IN  	STD_LOGIC;
		--DATA0_I			: IN 	STD_LOGIC_VECTOR(15 downto 0);
		--DATA1_I			: IN 	STD_LOGIC_VECTOR(15 downto 0);
		--DATA2_I			: IN 	STD_LOGIC_VECTOR(15 downto 0);
		--DATA3_I			: IN 	STD_LOGIC_VECTOR(15 downto 0);
		--DATA4_O			: OUT	STD_LOGIC_VECTOR(15 downto 0);
		--DATA5_O			: OUT 	STD_LOGIC_VECTOR(15 downto 0);
		--DATA6_O			: OUT	STD_LOGIC_VECTOR(15 downto 0);
		--DATA7_O			: OUT 	STD_LOGIC_VECTOR(15 downto 0);		
		--MEMADD_I		: IN 	STD_LOGIC_VECTOR(2 downto 0);
		--MEMDATA_I		: IN	STD_LOGIC_VECTOR(15 downto 0);
		--MEMDATA_O		: OUT	STD_LOGIC_VECTOR(15 downto 0);
		--EN_I			: IN 	STD_LOGIC;
		--RW_I			: IN 	STD_LOGIC
	--);
	--END COMPONENT;

	--COMPONENT PWM_GENERATOR IS 																		-- : PWM generator instanciation	
	--PORT (
		--RESET_I		: IN  	STD_LOGIC;
		--CLK_I      	: IN  	STD_LOGIC;
		--PWM0_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
		--PWM1_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
		--PWM2_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
		--PWM3_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
		--PWM0_O	: OUT  	STD_LOGIC;
		--PWM1_O	: OUT  	STD_LOGIC;
		--PWM2_O	: OUT  	STD_LOGIC;
		--PWM3_O	: OUT  	STD_LOGIC
		--);
	--END COMPONENT;

BEGIN
	 --internal oscillator
	--OSC: OSCH
    --GENERIC MAP (NOM_FREQ  => "133.00")																-- set clock to 133Mhz
    --PORT MAP (STDBY => '0', OSC => clk, SEDSTDBY => OPEN);

--	 Quadrature Encodeur interface
	--QEI0 : QEI
	--PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE0_CHA_i, QE_CHB_i => QE0_CHB_i, HOLD_I => SPI_SS_I, QE_COUNTER_o => qei_counter0);

	--QEI1 : QEI
	--PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE1_CHA_i, QE_CHB_i => QE1_CHB_i, HOLD_I => SPI_SS_I, QE_COUNTER_o => qei_counter1);
	
	--QEI2 : QEI
	--PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE2_CHA_i, QE_CHB_i => QE2_CHB_i, HOLD_I => SPI_SS_I, QE_COUNTER_o => qei_counter2);
	--QEI3 : QEI
	--PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE3_CHA_i, QE_CHB_i => QE3_CHB_i, HOLD_I => SPI_SS_I, QE_COUNTER_o => qei_counter3);

	--SPI : SIMPLE_SPI
	--PORT MAP (RESET_i => RESET_i, CLK_i => clk, SCLK_I => SPI_CLK_I, SS_I => SPI_SS_I, MOSI_I => SPI_MOSI_I, MISO_O => SPI_MISO_O, DATA_TO_TX_O => to_spi,
			--TX_LOAD_I => tx_load, RX_DATA_O => from_spi, IRQ_O => spi_irq);
	
			
	--MEM : MEMORY_MANAGER
		--PORT MAP (	RESET_I => RESET_I,	CLK_I => clk, DATA0_I => qei_counter0, DATA1_I => qei_counter1, DATA2_I => qei_counter2, DATA3_I => qei_counter3,
					--DATA4_O => pwm0_mem, DATA5_O => pwm1_mem, DATA6_O => pwm2_mem, DATA7_O => pwm3_mem, MEMADD_I => memory_address, MEMDATA_I => to_memory, MEMDATA_O => from_memory,
					--EN_I => memory_en, RW_I => memory_rw);
	
	--PWM : PWM_GENERATOR 
		--PORT MAP ( 	RESET_I => RESET_I,	CLK_I => clk, PWM0_VALUE_I => unsigned(pwm0_value), PWM1_VALUE_I => unsigned(pwm1_value), PWM2_VALUE_I => unsigned(pwm2_value),
					--PWM3_VALUE_I => unsigned(pwm3_value), PWM0_O => pwm0, PWM1_O => pwm1, PWM2_O => pwm2, PWM3_O => pwm3);

	
	--PROCESS(clk, RESET_I)											
	--BEGIN
		--IF (RESET_I = '1') THEN
			--machine_state <= waiting;								-- Start by waiting for packet from SPI
			--next_state <= address;									-- First packet will be address
			--memory_rw <= '0';
			--memory_address <= (OTHERS => '0');
			--to_memory <= (OTHERS => '0');
			--memory_en <= '0';
		--ELSIF( rising_edge(clk) ) THEN
			--CASE machine_state IS
				--WHEN waiting =>										--| waiting |--
					--memory_en <= '0';					
					--tx_load <= '0';
					--IF(spi_irq='1') THEN							-- New packet received
						--machine_state <= next_state;
					--END IF;
				--WHEN address =>										--| address |--
						--memory_rw <= from_spi(7);					-- bit 7 determine if set or get
						--memory_address <= from_spi(2 DOWNTO 0);		-- address is the four lsb bits				
						--IF(from_spi(7) = '1')THEN					-- if master want to write
							--next_state <= rx_data;					-- next packet received will be lsb
							--machine_state <= waiting;				-- wait for next packet
						--ELSE										-- else master want to read
							--IF(spi_irq='0') THEN					-- wait for TX empty
								--machine_state <= prepare_tx;			
							--END IF;												
						--END IF;
				--WHEN rx_data =>										--| rx_data |--
						--to_memory <= from_spi;						-- put lsb into buffer			
						--next_state <= address;						-- next packet received will be data
						--machine_state <= waiting;					-- wait for next packet
				--WHEN prepare_tx =>									--| prepare_tx |--
						--to_spi <= from_memory;						-- Sent data asked by the Master
						--tx_load <= '1';								-- latch data to send back to master
						--machine_state <= waiting;					
						--next_state <= end_of_transmission;			-- wait for transmission to end
				--WHEN end_of_transmission =>							--| end_of_transmission |--
						--tx_load <= '0';						
						--next_state <= address;						-- wait for next transaction
						--machine_state <= waiting;	
				--WHEN OTHERS => NULL;
			--END CASE;	
		--END IF;	
--END PROCESS;
	
--pwm0_sens 	<= 	pwm0_mem(15);										-- if msb = 1 then sens is Reverse
--pwm0_value 	<= 	pwm0_mem WHEN pwm0_sens = '0' ELSE					
				--STD_LOGIC_VECTOR(unsigned(not pwm0_mem) + 1); 		-- so pwm value need to be convert in unsigned
--PWM0_IN1_O 	<= 	not pwm0 WHEN pwm0_sens = '0' ELSE '1';				-- driven logic  of DRC8872
--PWM0_IN2_O 	<= 	not pwm0 WHEN pwm0_sens = '1' ELSE '1';

--pwm1_sens 	<= 	pwm1_mem(15);
--pwm1_value 	<= 	pwm1_mem WHEN pwm1_sens = '0' ELSE					-- same as pwm0
				--STD_LOGIC_VECTOR(unsigned(not pwm1_mem) + 1); 
--PWM1_IN1_O 	<= 	not pwm1 WHEN pwm1_sens = '0' ELSE '1';
--PWM1_IN2_O 	<= 	not pwm1 WHEN pwm1_sens = '1' ELSE '1';

--pwm2_sens 	<= 	pwm2_mem(15);
--pwm2_value 	<= 	pwm2_mem WHEN pwm2_sens = '0' ELSE					-- same as pwm0
				--STD_LOGIC_VECTOR(unsigned(not pwm2_mem) + 1);
--PWM2_IN1_O 	<= 	not pwm2 WHEN pwm2_sens = '0' ELSE '1';
--PWM2_IN2_O 	<= 	not pwm2 WHEN pwm2_sens = '1' ELSE '1';

--pwm3_sens 	<= 	pwm3_mem(15);
--pwm3_value 	<= 	pwm3_mem WHEN pwm3_sens = '0' ELSE					-- same as pwm0
				--STD_LOGIC_VECTOR(unsigned(not pwm3_mem) + 1);
--PWM3_IN1_O 	<= 	not pwm3 WHEN pwm3_sens = '0' ELSE '1';
--PWM3_IN2_O 	<= 	not pwm3 WHEN pwm3_sens = '1' ELSE '1';
PWM0_IN1_O 	<= 	'1';
PWM0_IN2_O 	<= 	'0';
PWM1_IN1_O 	<= 	'0';
PWM1_IN2_O 	<= 	'1'; 
PWM2_IN1_O 	<= 	'1'; 
PWM2_IN2_O 	<= 	'0';
PWM3_IN1_O 	<= 	'1'; 
PWM3_IN2_O 	<= 	'1'; 

END BEHAVIOR;