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

	g_addcharacter("peanut",
		"Peanut_Clone",
		{0.0,5000.0,0.0},
		"ai")

	g_addweapon("ka11","Peanut")
	g_addweapon("thompdad","Peanut_Clone")

	-- Initialize physics : gravity,friction,airfriction,speed,jumpspeed
	g_physics(10.0,30.0,0.1,4.0,20.0)

	-- Initialize camera (optional)
	g_lookat("Peanut",3.0)

end

--This function is called when the game needs to quit
function g_quit()

	g_print("Exiting game...\n")

end

