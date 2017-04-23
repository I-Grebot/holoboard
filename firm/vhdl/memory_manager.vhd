LIBRARY  ieee; 
  USE ieee.std_logic_1164.all; 
  USE ieee.numeric_std.all; 
	 
ENTITY MEMORY_MANAGER IS 
	PORT (
		RESET_I			: IN  	STD_LOGIC;
		CLK_I      		: IN  	STD_LOGIC;
		DATA0_I			: IN 	STD_LOGIC_VECTOR(15 downto 0);
		DATA1_I			: IN 	STD_LOGIC_VECTOR(15 downto 0);
		DATA2_I			: IN 	STD_LOGIC_VECTOR(15 downto 0);
		DATA3_I			: IN 	STD_LOGIC_VECTOR(15 downto 0);
		DATA4_O			: OUT	STD_LOGIC_VECTOR(15 downto 0);
		DATA5_O			: OUT 	STD_LOGIC_VECTOR(15 downto 0);
		DATA6_O			: OUT	STD_LOGIC_VECTOR(15 downto 0);
		DATA7_O			: OUT 	STD_LOGIC_VECTOR(15 downto 0);		
		MEMADD_I		: IN 	STD_LOGIC_VECTOR(2 downto 0);
		MEMDATA_I		: IN	STD_LOGIC_VECTOR(15 downto 0);
		MEMDATA_O		: OUT	STD_LOGIC_VECTOR(15 downto 0);
		EN_I			: IN 	STD_LOGIC;
		RW_I			: IN 	STD_LOGIC
	);
END MEMORY_MANAGER;

ARCHITECTURE BEHAVIOR OF MEMORY_MANAGER IS

	SIGNAL en_latched :  STD_LOGIC;

	BEGIN
	
	PROCESS(RESET_I, CLK_I)
	BEGIN
		IF (RESET_I = '1') THEN
			MEMDATA_O <= (OTHERS => '0');
			DATA4_O <= (OTHERS => '0');
			DATA5_O <= (OTHERS => '0');
			DATA6_O <= (OTHERS => '0');
			DATA7_O <= (OTHERS => '0');			
			en_latched <= '0';
		ELSIF( rising_edge(CLK_I) ) THEN
			en_latched	<= EN_I;
			CASE MEMADD_I IS
				WHEN "000" =>
					IF (RW_I='0')THEN
						MEMDATA_O <= DATA0_I;						
					END IF;
				WHEN "001" =>
					IF (RW_I='0')THEN
						MEMDATA_O <= DATA1_I;						
					END IF;
				WHEN "010" =>
					IF (RW_I='0')THEN
						MEMDATA_O <= DATA2_I;						
					END IF;
				WHEN "011" =>
					IF (RW_I='0')THEN
						MEMDATA_O <= DATA3_I;						
					END IF;
				WHEN "100" =>
					MEMDATA_O <= (OTHERS => '0');
					IF (RW_I='1' and en_latched='0' and EN_I='1')THEN
						DATA4_O <= MEMDATA_I(15 DOWNTO 0);
					END IF;
				WHEN "101" =>
					MEMDATA_O <= (OTHERS => '0');
					IF (RW_I='1' and en_latched='0' and EN_I='1')THEN
						DATA5_O <= MEMDATA_I(15 DOWNTO 0);
					END IF;
				WHEN "110" =>
					MEMDATA_O <= (OTHERS => '0');
					IF (RW_I='1' and en_latched='0' and EN_I='1')THEN
						DATA6_O <= MEMDATA_I(15 DOWNTO 0);
					END IF;	
				WHEN "111" =>
					MEMDATA_O <= (OTHERS => '0');
					IF (RW_I='1' and en_latched='0' and EN_I='1')THEN
						DATA7_O <= MEMDATA_I(15 DOWNTO 0);
					END IF;					
				WHEN OTHERS => NULL;
			END CASE;
		END IF;
	END PROCESS;
END BEHAVIOR;