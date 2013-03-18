
function toggle_interface()
	local stats = GUI:getWidget('stats')
	if stats ~= nil then
		stats:toggle()
	end
end

function toggle_map()
	local map = GUI:getWidget('minimap_container')
	if map ~= nil then
		map:toggle()
	end
end

function next_map()
	local map = GUI:getWidget('minimap')
	map:WidgetPicture(map:WidgetPicture() + 1)
end

function prev_map()
	local map = GUI:getWidget('minimap')
	map:WidgetPicture(map:WidgetPicture() - 1)
end