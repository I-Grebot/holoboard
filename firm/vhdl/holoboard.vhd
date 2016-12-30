 LIBRARY  ieee; 
	USE ieee.std_logic_1164.all; 
	USE ieee.numeric_std.all; 
LIBRARY lattice;
	USE lattice.components.all;
LIBRARY machxo2;
	USE machxo2.all;
  
ENTITY HOLOBOARD IS 
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
		PWM0_IN1_o	: OUT	STD_LOGIC;
		PWM0_IN2_o	: OUT	STD_LOGIC;
		PWM1_IN1_o	: OUT	STD_LOGIC;
		PWM1_IN2_o	: OUT	STD_LOGIC;
		PWM2_IN1_o	: OUT	STD_LOGIC;
		PWM2_IN2_o	: OUT	STD_LOGIC;
		PWM3_IN1_o	: OUT	STD_LOGIC;
		PWM3_IN2_o	: OUT	STD_LOGIC
		);
END HOLOBOARD;

ARCHITECTURE BEHAVIOR OF HOLOBOARD IS
	TYPE STATE_TYPE is (init1, init2, waiting, clear_irq, address, rx_data_lsb,rx_data_msb,wait_tx_empty,prepare_tx_lsb,prepare_tx_msb,end_of_transmission);
	
	SIGNAL clk  : STD_LOGIC;																			-- generale clock = 133 Mhz
   	SIGNAL qei_counter0, qei_counter1, qei_counter2, qei_counter3 : STD_LOGIC_VECTOR(15 DOWNTO 0);
	SIGNAL pwm0_mem, pwm1_mem, pwm2_mem, pwm3_mem : STD_LOGIC_VECTOR(15 DOWNTO 0);						-- pwm input for pwm generator
	SIGNAL pwm0_value, pwm1_value, pwm2_value, pwm3_value : STD_LOGIC_VECTOR(15 DOWNTO 0);				-- pwm input for pwm generator
	SIGNAL pwm0_sens, pwm1_sens, pwm2_sens, pwm3_sens : STD_LOGIC;										-- 0 = Forward ; 1 = Reverse
	SIGNAL pwm0, pwm1, pwm2, pwm3 : STD_LOGIC;															-- pwm output from pwm generator
	SIGNAL wb_cyc, wb_stb, wb_ack, wb_we, spi_irq : STD_LOGIC;											-- signals used for WISHBONE maangement
	SIGNAL wb_adr, wb_data_in, wb_data_out, rx_data : STD_LOGIC_VECTOR(7 DOWNTO 0);
	SIGNAL valid_cmd : STD_LOGIC;
	SIGNAL valid_address, valid_data : STD_LOGIC_VECTOR(7 DOWNTO 0);
	SIGNAL start_wb_com, wb_finish : STD_LOGIC;
	SIGNAL machine_state, next_state : STATE_TYPE;														-- state machine to handle SPI communication
	SIGNAL memory_address : STD_LOGIC_VECTOR(2 DOWNTO 0);
	SIGNAL memory_rw : STD_LOGIC;																		-- 0 = read ; 1 = write
	SIGNAL memory_en : STD_LOGIC;
	SIGNAL to_memory	: std_logic_vector(15 downto 0);
	SIGNAL from_memory	: std_logic_vector(15 downto 0);
	
   --internal oscillator
	COMPONENT OSCH
		GENERIC(
			NOM_FREQ: string := "53.20");
		PORT(
            STDBY    : IN  STD_LOGIC;
            OSC      : OUT STD_LOGIC;
            SEDSTDBY : OUT STD_LOGIC);
	END COMPONENT;
   
	COMPONENT SPI_BUS
		PORT (
			WB_CLK_I: IN  STD_LOGIC; 
			WB_RST_I: IN  STD_LOGIC; 
			WB_CYC_I: IN  STD_LOGIC; 
			WB_STB_I: IN  STD_LOGIC; 
			WB_WE_I: IN  STD_LOGIC; 
			WB_ADR_I: IN  STD_LOGIC_VECTOR(7 DOWNTO 0); 
			WB_DAT_I: IN  STD_LOGIC_VECTOR(7 DOWNTO 0); 
			WB_DAT_O: OUT  STD_LOGIC_VECTOR(7 DOWNTO 0); 
			WB_ACK_O: OUT  STD_LOGIC; 
			SPI_CLK: INOUT  STD_LOGIC; 
			SPI_MISO: INOUT  STD_LOGIC; 
			SPI_MOSI: INOUT  STD_LOGIC; 
			SPI_SCSN: IN  STD_LOGIC; 
			SPI_IRQ: OUT  STD_LOGIC);
		END COMPONENT;
   
   	COMPONENT QEI 
		PORT (
			RESET_I			: IN 	STD_LOGIC;
			CLK_I      		: IN  	STD_LOGIC;
			QE_CHA_I		: IN 	STD_LOGIC;
			QE_CHB_I		: IN 	STD_LOGIC;
			HOLD_I			: IN 	STD_LOGIC;
			QE_COUNTER_O	: OUT	STD_LOGIC_VECTOR(15 DOWNTO 0));  
   END COMPONENT;
   
	COMPONENT MEMORY_MANAGER IS 
		PORT (
			RESET_I			: IN  	STD_LOGIC;
			CLK_I      		: IN  	STD_LOGIC;
			DATA0_I			: IN 	STD_LOGIC_VECTOR(15 DOWNTO 0);
			DATA1_I			: IN 	STD_LOGIC_VECTOR(15 DOWNTO 0);
			DATA2_I			: IN 	STD_LOGIC_VECTOR(15 DOWNTO 0);
			DATA3_I			: IN 	STD_LOGIC_VECTOR(15 DOWNTO 0);
			DATA4_O			: OUT	STD_LOGIC_VECTOR(15 DOWNTO 0);
			DATA5_O			: OUT 	STD_LOGIC_VECTOR(15 DOWNTO 0);
			DATA6_O			: OUT 	STD_LOGIC_VECTOR(15 DOWNTO 0);
			DATA7_O			: OUT 	STD_LOGIC_VECTOR(15 DOWNTO 0);
			MEMADD_I		: IN 	STD_LOGIC_VECTOR(2 DOWNTO 0);
			MEMDATA_I		: IN	STD_LOGIC_VECTOR(15 DOWNTO 0);
			MEMDATA_O		: OUT	STD_LOGIC_VECTOR(15 DOWNTO 0);
			EN_I			: IN 	STD_LOGIC;
			RW_I			: IN 	STD_LOGIC
		);
	END COMPONENT;

	COMPONENT PWM_GENERATOR IS 
		PORT (
			RESET_I		: IN  	STD_LOGIC;
			CLK_I      	: IN  	STD_LOGIC;
			PWM0_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
			PWM1_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
			PWM2_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
			PWM3_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
			PWM0_O		: OUT STD_LOGIC;
			PWM1_O		: OUT STD_LOGIC;
			PWM2_O		: OUT STD_LOGIC;
			PWM3_O		: OUT STD_LOGIC
			);
	END COMPONENT;

BEGIN
	-- internal oscillator
	OSC: OSCH
    GENERIC MAP (NOM_FREQ  => "133.00")
    PORT MAP (STDBY => '0', OSC => clk, SEDSTDBY => OPEN);

	-- Quadrature Encodeur interface
	QEI0 : QEI
	PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE0_CHA_i, QE_CHB_i => QE0_CHB_i, HOLD_I => HOLD_QEx_I, QE_COUNTER_o => qei_counter0);

	QEI1 : QEI
	PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE1_CHA_i, QE_CHB_i => QE1_CHB_i, HOLD_I => HOLD_QEx_I, QE_COUNTER_o => qei_counter1);
	
	QEI2 : QEI
	PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE2_CHA_i, QE_CHB_i => QE2_CHB_i, HOLD_I => HOLD_QEx_I, QE_COUNTER_o => qei_counter2);
	
	QEI3 : QEI
	PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE3_CHA_i, QE_CHB_i => QE3_CHB_i, HOLD_I => HOLD_QEx_I, QE_COUNTER_o => qei_counter3);
	
	SPI : SPI_BUS
	PORT MAP ( 	WB_CLK_I => clk, WB_RST_I => RESET_i, WB_CYC_I => wb_cyc, WB_STB_I => wb_stb, WB_WE_I => wb_we, WB_ADR_I => wb_adr, WB_DAT_I => wb_data_out,
				WB_DAT_O => wb_data_in, WB_ACK_O => wb_ack, SPI_CLK => SPI_CLK_I, SPI_MISO => SPI_MISO_O, SPI_MOSI => SPI_MOSI_I, SPI_SCSN => SPI_SS_i,
				SPI_IRQ => spi_irq);
			
	MEM : MEMORY_MANAGER
		PORT MAP (	RESET_I => RESET_I,	CLK_I => clk, DATA0_I => qei_counter0, DATA1_I => qei_counter1, DATA2_I => qei_counter2, DATA3_I => qei_counter3,
					DATA4_O => pwm0_mem, DATA5_O => pwm1_mem, DATA6_O => pwm2_mem, DATA7_O => pwm3_mem,	MEMADD_I => memory_address, MEMDATA_I => to_memory, MEMDATA_O => from_memory,
					EN_I => memory_en, RW_I => memory_rw);
	
	PWM : PWM_GENERATOR 
		PORT MAP ( 	RESET_I => RESET_I,	CLK_I => clk, PWM0_VALUE_I => unsigned(pwm0_value), PWM1_VALUE_I => unsigned(pwm1_value), PWM2_VALUE_I => unsigned(pwm2_value), 
					PWM3_VALUE_I => unsigned(pwm3_value), PWM0_O => pwm0, PWM1_O => pwm1,	PWM2_O => pwm2, PWM3_O => pwm3);

	
	PROCESS(clk, RESET_I)											--| state machine |--
	BEGIN
		IF (RESET_I = '1') THEN
			machine_state <= init1;									-- : by initialisation
			next_state <= init1;							
			valid_data <= (OTHERS => '0');
			valid_cmd <= '0';										-- : read command by default
			valid_address <= X"5B";									-- : SPI Receive Data Register
			memory_rw <= '0';
			memory_address <= (OTHERS => '0');
			start_wb_com <= '0';
			to_memory <= (OTHERS => '0');
			memory_en <= '0';
		ELSIF( rising_edge(clk) ) THEN
			CASE machine_state IS
				WHEN init1 =>
					valid_cmd <= '1';						-- : write WISHBONE
					valid_address <= X"59";					-- : SPI transmit data register
					valid_data <= X"00";					-- : clear
					start_wb_com <= '1';
					IF(wb_finish = '1')THEN
						start_wb_com <= '0';
						machine_state <= clear_irq;
						next_state <= init2;
					END IF;
				WHEN init2 =>
						machine_state <= waiting;
						next_state <= address;
				WHEN waiting =>
					memory_en <= '0';
					IF(spi_irq='1') THEN							-- : a new data received
						valid_cmd <= '0';							-- : read WISHBONE
						valid_address <= X"5B";						-- : SPI receive data register
						start_wb_com <= '1';						-- : start WISHBONE communication
						IF(wb_finish = '1')THEN
							start_wb_com <= '0';						
							machine_state <= clear_irq;
						END IF;
					END IF;
				WHEN clear_irq =>
					valid_cmd <= '1';							-- : write WISHBONE
					valid_address <= X"5C";						-- : SPI interrupt status register
					valid_data <= X"1B";						-- : clear all interrupts
						start_wb_com <= '1';						-- : start WISHBONE communication
						IF(wb_finish = '1')THEN
							start_wb_com <= '0';						
							machine_state <= next_state;	
						END IF;						
				WHEN address =>			
						memory_rw <= rx_data(7);					-- : msb determine if set or get
						memory_address <= rx_data(2 DOWNTO 0);		-- : address is the four lsb bits				
						IF(rx_data(7) = '1')THEN					-- : if master want to write
							next_state <= rx_data_lsb;				-- : next data received will be lsb
							machine_state <= waiting;				-- : wait for next message
						ELSE
							machine_state <= wait_tx_empty;			-- : else master want to read
							next_state <= prepare_tx_lsb;			
						END IF;
				WHEN rx_data_lsb =>	
						to_memory(7 DOWNTO 0) <= rx_data;			-- : put lsb into buffer			
						next_state <= rx_data_msb;					-- : next data received will be lsb
						machine_state <= waiting;					-- : wait for next message
				WHEN rx_data_msb =>
						to_memory(15 DOWNTO 8) <= rx_data;			-- : put lsb into buffer			
						next_state <= address;						-- : next data received will be lsb
						machine_state <= waiting;					-- : wait for next message
						memory_en <= '1';
				WHEN wait_tx_empty =>
						valid_address <= X"5A";						-- : SPI status Register
						valid_cmd <= '0';							-- : read WISHBONE
						start_wb_com <= '1';						-- : start WISHBONE communication
						IF(wb_finish = '1')THEN
							start_wb_com <= '0';
							IF(rx_data(4) = '1')THEN
								machine_state <= next_state;
							END IF;
						END IF;
				WHEN prepare_tx_lsb =>
						valid_cmd <= '1';							-- : write WISHBONE
						valid_address <= X"59";						-- : SPI transmit data register
						valid_data <= from_memory(7 DOWNTO 0);		-- : Sent data asked by the Master
						start_wb_com <= '1';
						IF(wb_finish = '1')THEN
							start_wb_com <= '0';
							next_state <= prepare_tx_msb;			-- : next data received will be lsb
							machine_state <= waiting;				-- : wait for next message
						END IF;
				WHEN prepare_tx_msb =>
						IF(valid_cmd = '1' and start_wb_com = '0')THEN
							machine_state <= wait_tx_empty;
						ELSE
							valid_cmd <= '1';							-- : write WISHBONE
							valid_address <= X"59";						-- : SPI transmit data register
							valid_data <= from_memory(15 DOWNTO 8);		-- : Sent data asked by the Master
							start_wb_com <= '1';
							IF(wb_finish = '1')THEN
								start_wb_com <= '0';
								next_state <= end_of_transmission;		-- : next data received will be lsb
								machine_state <= waiting;				-- : wait for next message
							END IF;
						END  IF;
				WHEN end_of_transmission =>
							next_state <= address;						-- : next data received will be lsb
							machine_state <= waiting;					-- : wait for next message
				WHEN OTHERS => NULL;
			END CASE;	
		END IF;	
END PROCESS;

PROCESS(clk, RESET_I)
	VARIABLE wb_step : INTEGER RANGE 0 TO 3 :=0;
	BEGIN
	IF (RESET_I = '1') THEN
		wb_cyc <= '0';
		wb_stb <= '0';
		wb_we <= '0';										
		wb_adr <= (OTHERS => '0');
		rx_data <= (OTHERS => '0');
		wb_data_out <= (OTHERS => '0');
		wb_step := 0;
		wb_finish <= '0';
	ELSIF( rising_edge(clk) ) THEN
		CASE wb_step IS
			WHEN 0 =>
				IF(start_wb_com = '1')THEN
					wb_cyc <= '1';
					wb_stb <= '1';
					wb_we <= valid_cmd;
					wb_adr <= valid_address;
					IF(valid_cmd = '1')THEN
						wb_data_out <= valid_data;
					END IF;
					wb_step := 1;
				END IF;
			WHEN 1 =>
				IF(wb_ack = '1')THEN
					wb_step := 2;
				END IF;
			WHEN 2 =>
				wb_cyc <= '0';
				wb_stb <= '0';
				IF(valid_cmd = '0') THEN
					rx_data <= wb_data_in;
				END IF;
				wb_finish <= '1';
				wb_step := 3;
			WHEN 3 =>
				wb_finish <= '0';
				wb_step := 0;
			WHEN OTHERS =>  NULL;
		END CASE;
	END IF;
END PROCESS;

pwm0_sens 	<= 	pwm0_mem(15);
pwm0_value 	<= 	pwm0_mem WHEN pwm0_sens = '0' ELSE
				STD_LOGIC_VECTOR(unsigned(not pwm0_mem) + 1); 
PWM0_IN1_O 	<= 	not pwm0 WHEN pwm0_sens = '0' ELSE '1';
PWM0_IN2_O 	<= 	not pwm0 WHEN pwm0_sens = '1' ELSE '1';

pwm1_sens 	<= 	pwm1_mem(15);
pwm1_value 	<= 	pwm1_mem WHEN pwm1_sens = '0' ELSE
				STD_LOGIC_VECTOR(unsigned(not pwm1_mem) + 1); 
PWM1_IN1_O 	<= 	not pwm1 WHEN pwm1_sens = '0' ELSE '1';
PWM1_IN2_O 	<= 	not pwm1 WHEN pwm1_sens = '1' ELSE '1';

pwm2_sens 	<= 	pwm2_mem(15);
pwm2_value 	<= 	pwm2_mem WHEN pwm2_sens = '0' ELSE
				STD_LOGIC_VECTOR(unsigned(not pwm2_mem) + 1);
PWM2_IN1_O 	<= 	not pwm2 WHEN pwm2_sens = '0' ELSE '1';
PWM2_IN2_O 	<= 	not pwm2 WHEN pwm2_sens = '1' ELSE '1';

pwm3_sens 	<= 	pwm3_mem(15);
pwm3_value 	<= 	pwm3_mem WHEN pwm3_sens = '0' ELSE
				STD_LOGIC_VECTOR(unsigned(not pwm3_mem) + 1);
PWM3_IN1_O 	<= 	not pwm3 WHEN pwm3_sens = '0' ELSE '1';
PWM3_IN2_O 	<= 	not pwm3 WHEN pwm3_sens = '1' ELSE '1';

END BEHAVIOR;