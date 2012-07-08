


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


function Console:widget()
	if self.console == nil then
		self.console = Interface.getWidgetByName("console")
		self.lines = self.console:Lines()
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

function Console:process( keynum, char )
	if keynum == keys.up then
		self.string = ilua.prev_history()
	elseif keynum == keys.down then
		self.string = ilua.next_history()
	end

	if keynum == keys.enter then
		table.insert(self.history, '> ' .. self.string .. '\n')
		local process = ilua.process(self.string)
		local lines = {}
		process:gsub(
			string.format("([^%s]+)", "\n"),
			function(c) self.history[#self.history+1] = c .. '\n' end
		)
		self.string = ''
		self.cursor = 0
	elseif keynum == keys.backspace then
		if self.cursor >= 1 then
			local start = self.string:sub(0, self.cursor - 1)
			local send = self.string:sub(self.cursor + 1)
			self.string = start .. send
			self.cursor = self.cursor - 1
		end
	elseif keynum == keys.tab then
		local completion = ilua.complete(self.string)
		if type(completion) == "string" then
			self.string = completion
		elseif type(completion) == "table" then
			local maxlen = 0
			for k,v in pairs(completion) do
				if maxlen < #v then maxlen = #v end
			end
			local compl = ""
			local cols = 3
			if cols > #completion then cols = #completion end
			table.insert(self.history, self.prompt .. self.string .. '\n')
			local lines = math.ceil(#completion / cols)
			cols = cols - 1
			for j = 1, lines do
				for i=0, cols do
					if completion[j + cols * i] ~= nil then
						compl = compl .. string.format("%-" .. maxlen + 1 .. "s", completion[j + cols * i])
					end
				end
				table.insert(self.history, compl .. '\n')
				compl = ""
			end
		end
		self.cursor = self.string:len()
	elseif keynum == keys.pgdown then
		self.offset = self.offset - 3
		if self.offset < 0 then self.offset = 0 end
	elseif keynum == keys.pgup then
		self.offset = self.offset + 3
		local hcount = table.getn(self.history) - self.lines
		if self.offset > hcount then self.offset = hcount end
	elseif keynum == keys.right then
		self.cursor = self.cursor + 1
	elseif keynum == keys.left then
		self.cursor = self.cursor - 1
	else
		self.cursor = self.cursor + 1
		local start = self.string:sub(0, self.cursor - 1)
		local send = self.string:sub(self.cursor)
		self.string = start .. char .. send
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
	self:widget():setText(out .. self.prompt .. self.string .. ' ')
	self:widget():Cursor(out:len() + self.prompt:len() + self.cursor)
end
