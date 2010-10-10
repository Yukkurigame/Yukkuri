require("data/scripts/widgets")

LoadAllConfigs("tiles")
LoadAllConfigs("map")
LoadAllConfigs("widget")
LoadAllConfigs("entity")

fps = Widget.create("fps")
time = Widget.create("time")
stats = Widget.create("pstats")
schild = stats:children( )
if schild then
	for i,v in pairs(schild) do
		print(i,v)
	end
end


CreateUnit(1)