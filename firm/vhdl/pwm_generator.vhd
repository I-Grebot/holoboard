LIBRARY  ieee; 
  USE ieee.std_logic_1164.all; 
  USE ieee.numeric_std.all; 

	 
ENTITY PWM_GENERATOR IS 
	PORT (
		RESET_I		: IN  	STD_LOGIC;
		CLK_I      	: IN  	STD_LOGIC;
		PWM0_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
		PWM1_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
		PWM2_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
		PWM3_VALUE_I: IN UNSIGNED(15 DOWNTO 0);
		PWM0_O	: OUT  	STD_LOGIC;
		PWM1_O	: OUT  	STD_LOGIC;
		PWM2_O	: OUT  	STD_LOGIC;
		PWM3_O	: OUT  	STD_LOGIC
		);
END PWM_GENERATOR;

ARCHITECTURE BEHAVIOR OF PWM_GENERATOR IS

	SIGNAL counter : UNSIGNED(15 DOWNTO 0);
	CONSTANT MAX_VALUE : UNSIGNED(15 DOWNTO 0) := to_unsigned(6649,16);
	BEGIN
		PROCESS(RESET_I, CLK_I)
			BEGIN
				IF (RESET_I = '1') THEN
					counter <= (OTHERS => '0');			
				ELSIF (rising_edge(CLK_I)) THEN
					IF (counter = MAX_VALUE) THEN
						counter <= (OTHERS => '0');
					ELSE
						counter <= counter + 1;
					END IF;
				END IF;
		END PROCESS;
				
		PWM0_O <= 	'0' WHEN RESET_I='1' ELSE
					'0' WHEN PWM0_VALUE_I = to_unsigned(0,16) ELSE 
					'1' WHEN PWM0_VALUE_I >= MAX_VALUE ELSE
					'1' WHEN counter <= PWM0_VALUE_I ELSE '0';
					
		PWM1_O <= 	'0' WHEN RESET_I='1' ELSE
					'0' WHEN PWM1_VALUE_I = to_unsigned(0,16) ELSE 
					'1' WHEN PWM1_VALUE_I >= MAX_VALUE ELSE
					'1' WHEN counter <= PWM1_VALUE_I ELSE '0';
					
		PWM2_O <= 	'0' WHEN RESET_I='1' ELSE
					'0' WHEN PWM2_VALUE_I = to_unsigned(0,16) ELSE 
					'1' WHEN PWM2_VALUE_I >= MAX_VALUE ELSE
					'1' WHEN counter <= PWM2_VALUE_I ELSE '0';	

		PWM3_O <= 	'0' WHEN RESET_I='1' ELSE
					'0' WHEN PWM3_VALUE_I = to_unsigned(0,16) ELSE 
					'1' WHEN PWM3_VALUE_I >= MAX_VALUE ELSE
					'1' WHEN counter <= PWM3_VALUE_I ELSE '0';	
END BEHAVIOR;