LIBRARY  ieee; 
	USE ieee.std_logic_1164.all; 
	USE ieee.numeric_std.all; 
LIBRARY lattice;
	USE lattice.all;
LIBRARY machxo2;
	USE machxo2.all;
  
ENTITY HOLOBOARD IS 
	PORT (
		RESET_I		: IN  	STD_LOGIC;
		SPI_CLK_I   : IN	STD_LOGIC;
		SPI_SS_I    : IN 	STD_LOGIC;
		SPI_MOSI_I  : IN	STD_LOGIC;
		SPI_MISO_O	: OUT	STD_LOGIC;
		QE0_CHA_I	: IN 	STD_LOGIC;
		QE0_CHB_I	: IN 	STD_LOGIC;
		QE1_CHA_I	: IN 	STD_LOGIC;
		QE1_CHB_I	: IN 	STD_LOGIC;
		QE2_CHA_I	: IN 	STD_LOGIC;
		QE2_CHB_I	: IN 	STD_LOGIC;
		--QE3_CHA_I	: IN 	STD_LOGIC;
		--QE3_CHB_I	: IN 	STD_LOGIC;
		PWM0_IN1_O	: OUT	STD_LOGIC;
		PWM0_IN2_O	: OUT	STD_LOGIC;
		PWM1_IN1_O	: OUT	STD_LOGIC;
		PWM1_IN2_O	: OUT	STD_LOGIC;
		PWM2_IN1_O	: OUT	STD_LOGIC;
		PWM2_IN2_O	: OUT	STD_LOGIC		
		--PWM3_IN1_O	: OUT	STD_LOGIC;
		--PWM3_IN2_O	: OUT	STD_LOGIC
		);
END HOLOBOARD;

ARCHITECTURE BEHAVIOR OF HOLOBOARD IS

	SIGNAL clk  : STD_LOGIC;																		-- generale clock = 133 Mhz
   	SIGNAL qei_counter0, qei_counter1, qei_counter2 : STD_LOGIC_VECTOR(11 DOWNTO 0);	-- qei_counters
	SIGNAL pwm0_mem, pwm1_mem, pwm2_mem : STD_LOGIC_VECTOR(11 DOWNTO 0);					-- pwm mem (signed)
	SIGNAL pwm0_value, pwm1_value, pwm2_value : UNSIGNED(10 DOWNTO 0);			-- pwm input for pwm generator
	SIGNAL pwm0_sens, pwm1_sens, pwm2_sens: STD_LOGIC;									-- 0 = Forward ; 1 = Reverse
	SIGNAL pwm0, pwm1, pwm2: STD_LOGIC;														-- pwm output from pwm generator
	SIGNAL memory_address : STD_LOGIC_VECTOR(3 DOWNTO 0);
	SIGNAL memory_rw : STD_LOGIC;																	-- 0 = read ; 1 = write
	SIGNAL to_spi	: std_logic_vector(15 downto 0);												-- data to send with SPI
	SIGNAL from_spi	: std_logic_vector(15 downto 0);												-- data received from SPI
	SIGNAL spi_irq, spi_irq_latched, tx_load, address_received : STD_LOGIC;
	SIGNAL data_received : STD_LOGIC;
   
	COMPONENT OSCH																					-- : internal oscillator				
		GENERIC(
			NOM_FREQ: string := "53.20");
		PORT(
            STDBY    : IN  STD_LOGIC;
            OSC      : OUT STD_LOGIC;
            SEDSTDBY : OUT STD_LOGIC);
	END COMPONENT;
   
	COMPONENT SIMPLE_SPI IS 																		-- : SPI instanciation
		PORT (		
			RESET_I			:	IN	STD_LOGIC;
			CLK_I     		:	IN	STD_LOGIC;
			SCLK_I   		:	IN	STD_LOGIC;
			SS_I    		:	IN	STD_LOGIC;
			MOSI_I  		:	IN	STD_LOGIC;
			MISO_O			:	OUT	STD_LOGIC;
			DATA_TO_TX_O  	: 	IN 	STD_LOGIC_VECTOR(15 DOWNTO 0);
			TX_LOAD_I		:	IN	STD_LOGIC;
			RX_DATA_O	 	: 	OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
			IRQ_O			:	OUT	STD_LOGIC
		);
	END COMPONENT;
    
	COMPONENT RAM is
    port (
        Data: in  std_logic_vector(11 downto 0); 
        Clock: in  std_logic; 
        WE: in  std_logic; 
        ClockEn1: in  std_logic;
		ClockEn2: in  std_logic;		
        Q: out  std_logic_vector(11 downto 0));
	END COMPONENT;

	COMPONENT PWM_GENERATOR IS 																		-- : PWM generator instanciation	
	PORT (
		RESET_I		: IN  	STD_LOGIC;
		CLK_I      	: IN  	STD_LOGIC;
		PWM0_VALUE_I: IN UNSIGNED(10 DOWNTO 0);
		PWM1_VALUE_I: IN UNSIGNED(10 DOWNTO 0);
		PWM2_VALUE_I: IN UNSIGNED(10 DOWNTO 0);
--		PWM3_VALUE_I: IN UNSIGNED(10 DOWNTO 0);
		PWM0_O	: OUT  	STD_LOGIC;
		PWM1_O	: OUT  	STD_LOGIC;
		PWM2_O	: OUT  	STD_LOGIC
		--PWM3_O	: OUT  	STD_LOGIC
);
	END COMPONENT;
	
	COMPONENT QEI IS 
	PORT (
		RESET_I			: IN  	STD_LOGIC;
		CLK_I      		: IN  	STD_LOGIC;
		QE_CHA_I		: IN 	STD_LOGIC;
		QE_CHB_I		: IN 	STD_LOGIC;
		HOLD_I			: IN 	STD_LOGIC;
		QE_COUNTER_O	: OUT	STD_LOGIC_VECTOR(11 downto 0)
		);
	END COMPONENT;

BEGIN-- internal oscillator
	OSC: OSCH
    GENERIC MAP (NOM_FREQ  => "133.00")																-- set clock to 133Mhz
    PORT MAP (STDBY => '0', OSC => clk, SEDSTDBY => OPEN);

-- Quadrature Encodeur interface
	QEI0 : QEI
	PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE0_CHA_i, QE_CHB_i => QE0_CHB_i, HOLD_I => SPI_SS_I, QE_COUNTER_o => qei_counter0);

	QEI1 : QEI
	PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE1_CHA_i, QE_CHB_i => QE1_CHB_i, HOLD_I => SPI_SS_I, QE_COUNTER_o => qei_counter1);
	
	QEI2 : QEI
	PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE2_CHA_i, QE_CHB_i => QE2_CHB_i, HOLD_I => SPI_SS_I, QE_COUNTER_o => qei_counter2);
	--QEI3 : QEI
	--PORT MAP (RESET_i => RESET_i, CLK_i => clk, QE_CHA_i => QE3_CHA_i, QE_CHB_i => QE3_CHB_i, HOLD_I => SPI_SS_I, QE_COUNTER_o => qei_counter3);

	SPI : SIMPLE_SPI
	PORT MAP (RESET_i => RESET_i, CLK_i => clk, SCLK_I => SPI_CLK_I, SS_I => SPI_SS_I, MOSI_I => SPI_MOSI_I, MISO_O => SPI_MISO_O, DATA_TO_TX_O => to_spi,
			TX_LOAD_I => tx_load, RX_DATA_O => from_spi, IRQ_O => spi_irq);
			
	MEM1 : RAM
	PORT MAP (Data(11 downto 0)=> from_spi(11 downto 0), Clock=> clk, WE=>memory_rw, ClockEn1=>memory_address(2), ClockEn2=>data_received, Q(11 downto 0)=>pwm0_mem);
	
	MEM2 : RAM
	PORT MAP (Data(11 downto 0)=> from_spi(11 downto 0), Clock=> clk, WE=>memory_rw, ClockEn1=>memory_address(1), ClockEn2=>data_received, Q(11 downto 0)=>pwm1_mem);
	
	MEM3 : RAM
	PORT MAP (Data(11 downto 0)=> from_spi(11 downto 0), Clock=> clk, WE=>memory_rw, ClockEn1=>memory_address(0), ClockEn2=>data_received, Q(11 downto 0)=>pwm2_mem);
--	MEM4 : RAM
--	PORT MAP (Data(11 downto 0)=> from_spi(11 downto 0), Clock=> clk, WE=>memory_rw, ClockEn1=>memory_address(0), ClockEn2=>data_received, Q(11 downto 0)=>pwm2_mem);


	PWM : PWM_GENERATOR 
	PORT MAP ( 	RESET_I => RESET_I,	CLK_I => clk, PWM0_VALUE_I => pwm0_value, PWM1_VALUE_I => pwm1_value, PWM2_VALUE_I => pwm2_value,
				PWM0_O => pwm0, PWM1_O => pwm1, PWM2_O => pwm2);

	PROCESS(clk, RESET_I)											
	BEGIN
		IF (RESET_I = '1') THEN
			memory_rw <= '0';
			memory_address <= (OTHERS => '0');
			address_received <= '0';
			spi_irq_latched <= '1';
			data_received <= '0';
		ELSIF( rising_edge(clk) ) THEN
			spi_irq_latched <= spi_irq;
			data_received <= '0';
			IF(spi_irq = '1' and spi_irq_latched = '0')THEN
				IF(address_received = '0') THEN
					memory_rw <= from_spi(7);					-- bit 7 determine if set or get
					memory_address <= from_spi(3 DOWNTO 0);		-- address is the three lsb bits		
					address_received <= '1';					-- set the received flag
					if(from_spi(7) = '0' and from_spi(0) = '1')THEN
						to_spi(11 downto 0) <= qei_counter2;
						tx_load <= '1';
					elsif(from_spi(7) = '0' and from_spi(1) = '1')THEN
						to_spi(11 downto 0) <= qei_counter1;
						tx_load <= '1';	
					elsif(from_spi(7) = '0' and from_spi(2) = '1')THEN
						to_spi(11 downto 0) <= qei_counter0;
						tx_load <= '1';							
					END IF;
				ELSE
					data_received <= '1';
					address_received <= '0';					-- clear the received flag
					tx_load <= '0';
				END IF;
			END IF;
		END IF;	
END PROCESS;

pwm0_sens 	<= 	pwm0_mem(11);																-- if msb = 1 then sens is Reverse
pwm0_value 	<= 	unsigned(pwm0_mem(10 downto 0));									-- so pwm value need to be convert in unsigned
PWM0_IN1_O 	<= 	not pwm0 WHEN pwm0_sens = '0' ELSE '1';										-- driven logic  of DRC8872
PWM0_IN2_O 	<= 	not pwm0 WHEN pwm0_sens = '1' ELSE '1';

pwm1_sens 	<= 	pwm1_mem(11);
pwm1_value 	<= 	unsigned(pwm1_mem(10 downto 0));					-- same as pwm0
PWM1_IN1_O 	<= 	not pwm1 WHEN pwm1_sens = '0' ELSE '1';
PWM1_IN2_O 	<= 	not pwm1 WHEN pwm1_sens = '1' ELSE '1';

pwm2_sens 	<= 	pwm2_mem(11);
pwm2_value 	<= 	unsigned(pwm2_mem(10 downto 0));					-- same as pwm0
PWM2_IN1_O 	<= 	not pwm2 WHEN pwm2_sens = '0' ELSE '1';
PWM2_IN2_O 	<= 	not pwm2 WHEN pwm2_sens = '1' ELSE '1';

--pwm3_sens 	<= 	pwm3_mem(11);
--pwm3_value 	<= 	unsigned(pwm3_mem(10 downto 0));					-- same as pwm0
--PWM3_IN1_O 	<= 	not pwm3 WHEN pwm3_sens = '0' ELSE '1';
--PWM3_IN2_O 	<= 	not pwm3 WHEN pwm3_sens = '1' ELSE '1';
END BEHAVIOR;