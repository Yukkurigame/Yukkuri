function table.dump(self)
    local result = tostring(self)..' {\n';
    local level, scope = 1, {};
    local map = {[true] = 'true', [false] = 'false'};
    local function _dump(t)
        local mt = getmetatable(t);
        local __tostring = nil;
        if mt then
            __tostring = mt.__tostring;
            mt.__tostring = nil;
        end;        

        local id = tostring(t);
        local tab = ('    '):rep(level);

        if scope[id] and scope[id] <= level then
            result = ('%s%s*RECURSION*\n'):format(result, tab);
            return;
        end;

        for k, v in pairs(t) do
            if type(v) ~= 'table' then
                result = ('%s%s[%s] => (%s)%s\n'):format(result, tab, k, type(v), tostring(map[v] or v));
            else
                result = ('%s%s[%s] => (%s)%s {\n'):format(result, tab, k, type(v), id);
                level = (level + 1);
                scope[id] = level;

                _dump(v);

                result = ('%s%s}\n\n'):format(result, tab);
                level = (level - 1);

                if level < 2 then
                    scope = {};
                end;
            end;
        end;

        if mt then
        	mt.__tostring = __tostring;
        end;
    end;

    _dump(self);
    return result..'}';
end;