LIBRARY  ieee; 
  USE ieee.std_logic_1164.all; 
  USE ieee.std_logic_unsigned.all; 
  USE ieee.numeric_std.all; 
  
ENTITY SIMPLE_SPI IS 
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
END SIMPLE_SPI;

ARCHITECTURE BEHAVIORAL OF SIMPLE_SPI IS

	SIGNAL rx_data 				: STD_LOGIC_VECTOR(15 DOWNTO 0);
	SIGNAL index				: NATURAL RANGE 0 TO 15;
	SIGNAL tx_data 				: STD_LOGIC_VECTOR(15 DOWNTO 0);
	SIGNAL sclk_latched 		: STD_LOGIC;
	SIGNAL sclk_buffered 		: STD_LOGIC;
	SIGNAL ss_latched 			: STD_LOGIC;
	SIGNAL mosi_latched			: STD_LOGIC;
	SIGNAL tx_load_latched		: STD_LOGIC;
	SIGNAL tx_load_buffered		: STD_LOGIC;
	
	BEGIN
	
	PROCESS(CLK_i, RESET_i)	
		BEGIN
			IF(RESET_i = '1') THEN
				rx_data <= (OTHERS => '0');
				index <= 0;
				tx_data <= (OTHERS => '0');
				IRQ_O <= '1';
				sclk_latched <= '0';
				sclk_buffered <= '0';
				ss_latched <= '0';
				mosi_latched <= '0';
				tx_load_latched <= '0';
			ELSIF( rising_edge(CLK_I) ) THEN
				sclk_latched <= SCLK_I;
				sclk_buffered <= sclk_latched;
				ss_latched <= SS_I;
				mosi_latched <= MOSI_i;
				tx_load_latched <= TX_LOAD_I;
				tx_load_buffered <= tx_load_latched;
				
				IF(tx_load_buffered = '0' and tx_load_latched = '1') THEN
					tx_data <= DATA_TO_TX_O;
				END IF;

				IF( ss_latched = '0' ) THEN
					IF(sclk_buffered = '0' and sclk_latched = '1') THEN
						IRQ_O <= '0';
						rx_data <= rx_data(14 DOWNTO 0) & mosi_latched;
					ELSIF(sclk_buffered = '1' and sclk_latched = '0') THEN
						tx_data <= tx_data(14 DOWNTO 0) & '0';
					END IF;
				ELSE
					IRQ_O <= '1';
				END IF;
			END IF;
	END PROCESS;

-- Combinational assignments
	MISO_O <= tx_data(15);
	RX_DATA_O <= rx_data;
	
END BEHAVIORAL;