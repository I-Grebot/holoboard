LIBRARY  ieee; 
  USE ieee.std_logic_1164.all; 
  USE ieee.numeric_std.all; 
	 
ENTITY QEI IS 
	PORT (
		RESET_I			: IN  	STD_LOGIC;
		CLK_I      		: IN  	STD_LOGIC;
		QE_CHA_I		: IN 	STD_LOGIC;
		QE_CHB_I		: IN 	STD_LOGIC;
		HOLD_I			: IN 	STD_LOGIC;
		QE_COUNTER_O	: OUT	STD_LOGIC_VECTOR(15 downto 0)
		);
END QEI;

ARCHITECTURE BEHAVIOR OF QEI IS

	SIGNAL counter 		: SIGNED(15 downto 0);
	SIGNAL cha_latched 	: STD_LOGIC;
	SIGNAL chb_latched 	: STD_LOGIC;
	SIGNAL hold_latched	: STD_LOGIC;

	BEGIN
	
	PROCESS(RESET_I, CLK_I)
		BEGIN
			IF (RESET_I = '1') THEN
				counter <= (OTHERS => '0');
				cha_latched <= '0';
				chb_latched <= '0';
			ELSIF( rising_edge(CLK_I) ) THEN
				cha_latched <= QE_CHA_I;
				chb_latched <= QE_CHB_I;
				IF(cha_latched ='0' AND QE_CHA_I ='1' AND QE_CHB_I ='0') OR 
				  (cha_latched ='1' AND QE_CHA_I ='0' AND QE_CHB_I ='1') OR
				  (chb_latched ='0' AND QE_CHB_I ='1' AND QE_CHA_I ='1') OR
				  (chb_latched ='1' AND QE_CHB_I ='0' AND QE_CHA_I ='0') THEN
					counter <= counter + 1;
				ELSIF(cha_latched ='0' AND QE_CHA_I ='1' AND QE_CHB_I ='1') OR 
					(cha_latched ='1' AND QE_CHA_I ='0' AND QE_CHB_I ='0') OR
					(chb_latched ='0' AND QE_CHB_I ='1' AND QE_CHA_I ='0') OR
					(chb_latched ='1' AND QE_CHB_I ='0' AND QE_CHA_I ='1') THEN
					counter <= counter - 1;
				END IF;
			END IF;
	END PROCESS;
	PROCESS(RESET_I, CLK_I)
		BEGIN
			IF (RESET_I = '1') THEN
				QE_COUNTER_O <= (OTHERS => '0');
				hold_latched <= '0';
			ELSIF( rising_edge(CLK_I) ) THEN
				hold_latched <= HOLD_I;
				IF(hold_latched ='0' AND HOLD_I ='1')THEN
					QE_COUNTER_O <= std_logic_vector(counter);
				END IF;
			END IF;
	END PROCESS;

END BEHAVIOR;