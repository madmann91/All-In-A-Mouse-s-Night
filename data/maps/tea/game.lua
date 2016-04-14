--This function is called once the map is loaded
function g_init()

	g_print("Initializing game...\n")

	-- Call game libraries
	g_loadlibrary("lib\\characters.lua")
	g_loadlibrary("lib\\weapons.lua")

	-- Add characters
	g_addcharacter("peanut",
		"Peanut",
		{0.0,500.0,0.0},
		"user")

	g_addweapon("magum","Peanut")

	for i=0,5 do
		g_addcharacter("rat",
			"Rat"..i,
			{0.0,500.0+i*6,0.0},
			"ai")
		if i>2 then
			g_addweapon("ka11","Rat"..i)
		else
			g_addweapon("m5","Rat"..i)
		end
	end

	-- Initialize physics : gravity,friction,airfriction,speed,jumpspeed
	g_physics(10.0,30.0,0.1,4.0,20.0)

	-- Initialize camera (optional)
	g_lookat("Peanut",3.0)

end

--This function is called when the game needs to quit
function g_quit()

	g_print("Exiting game...\n")

end

