

function Console:key_up()
	self.string = ilua.prev_history()
	self.cursor = self.string:len()
end


function Console:key_down()
	self.string = ilua.next_history()
	self.cursor = self.string:len()
end


function Console:key_enter()
	table.insert(self.history, '> ' .. self.string .. '\n')
	local process = ilua.process(self.string)
	local lines = {}
	process:gsub(
	string.format("([^%s]+)", "\n"),
		function(c) self.history[#self.history+1] = c .. '\n' end
	)
	self.string = ''
	self.cursor = 0
end


function Console:key_backspace()
	if self.cursor >= 1 then
		local start = self.string:sub(0, self.cursor - 1)
		local send = self.string:sub(self.cursor + 1)
		self.string = start .. send
		self.cursor = self.cursor - 1
	end
end


function Console:key_delete()
	local start = self.string:sub(0, self.cursor)
	local send = self.string:sub(self.cursor + 2)
	self.string = start .. send
end


function Console:key_tab()
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
		local multipler = cols
		table.insert(self.history, self.prompt .. self.string .. '\n')
		local lines = math.ceil(#completion / cols)
		if multipler > lines then multipler = lines end
		for j = 1, lines do
			for i=0, cols - 1 do
				if completion[j + multipler * i] ~= nil then
					compl = compl .. string.format("%-" .. maxlen + 1 .. "s", completion[j + multipler * i])
				end
			end
			table.insert(self.history, compl .. '\n')
			compl = ""
		end
	end
	self.cursor = self.string:len()
end


function Console:key_pgdown()
	self.offset = self.offset - 3
	if self.offset < 0 then self.offset = 0 end
end


function Console:key_pgup()
	self.offset = self.offset + 3
	local hcount = table.getn(self.history) - self.lines
	if self.offset > hcount then self.offset = hcount end
end


function Console:key_right()
	self.cursor = self.cursor + 1
end


function Console:key_left()
	self.cursor = self.cursor - 1
end
