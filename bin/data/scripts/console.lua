
Console = {
	cursor = 0,
	string = '',
	offset = 0,
	prompt = '> ',
	-- Output log. Commands log in iLua console history
	history = {},
}
Console.__index = Console
Console.iLua = require('data/scripts/ilua')
require('data/scripts/consolekeys')


function Console:widget()
	if self.console == nil then
		local console = Interface.getWidgetByName("console")
		if console == nil then
			console = Interface.createWidget("widget_console", constants.wtText)
			console:load({ name = "console",
				align = bit.bor(constants.waLEFT, constants.waTOP),
				height = 200, width = 300, depth = 2, bgcolor = {0, 0, 0, 127},
				text = {
					x = 5, y = -5, size = 18, color = {255,255,255},
					face = "./LiberationSerif-Regular.ttf", lineheight = 0.75,
					align = bit.bor(constants.waLEFT, constants.waTOP),
				}
			})
			local conf_video = configs:get( 'video', 'config_general', 'config' )
			console:resize(conf_video['window_width'] - 1, 300)
			console:toggle()
		end
		self.console = console
		self.lines = console:Lines()
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
		self:widget():CursorVisible(true)
	else
		Input.set( self.old_bindings )
		Input.setReciever(nil)
	end
end

function keyname_by_code(keynum)
	for k,v in pairs(keys) do
		if v == keynum then
			return k
		end
	end
end

function Console:process(keynum, char)
	local keyname = keyname_by_code(keynum)
	local func = nil
	if keyname ~= nil then
		func = self["key_" .. keyname]
	end
	if type(func) == "function" then
		func(self, char)
	else
		if char ~= "" then
			self.cursor = self.cursor + 1
			local start = self.string:sub(0, self.cursor - 1)
			local send = self.string:sub(self.cursor)
			self.string = start .. char .. send
		end
	end

	-- Print lines of log
	local out = ''
	local count = table.getn(self.history)
	if count > 0 then
		-- Start line
		local number = 0
		if count > self.lines then
			number = count - self.lines
		end
		for line = number - self.offset, number + self.lines - self.offset do
			if line < count then
				out = out .. self.history[line + 1]
			end
		end
	end
	if self.cursor < 0 then self.cursor = 0 end
	if self.cursor > self.string:len() then
		self.cursor = self.string:len()
	end
	self:widget():WidgetText(out .. self.prompt .. self.string .. ' ')
	self:widget():Cursor(out:len() + self.prompt:len() + self.cursor)
end
