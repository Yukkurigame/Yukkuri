
function toggle_interface()
	local stats = Interface.getWidgetByName("stats")
	stats:toggle()
end

function toggle_console()
	Console:toggle()
end


Console = {
	cursor = 0,
	string = "",
}
Console.__index = Console

function Console:widget()
	if self.console == nil then
		self.console = Interface.getWidgetByName("console")
		self.line = self.console:LineSize()
	end
	return self.console
end

function Console:toggle()	
	if self:widget():toggle() then
		self.old_bindings = Input.getName()
		Input.set("bindings_console")
		Input.setReciever( function(keynum, down, char)
			if down == 1 then self:process(keynum, char) end
			end
		)
	else
		Input.set( self.old_bindings )
		Input.setReciever(nil)
	end 
end

function Console:process( keynum, char )
	if keynum == keys.enter then
		char = '\n'
	elseif keynum == keys.backspace then
		char = ''
		self.string = string.sub(self.string, 1, -2) 
	end
	self.string = self.string .. char
	self:widget():setText(self.string)
end