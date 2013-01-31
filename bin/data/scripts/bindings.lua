
function toggle_interface()
	local stats = GUI:getWidget('stats')
	if stats ~= nil then
		stats:toggle()
	end
end

