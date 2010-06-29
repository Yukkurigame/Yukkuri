
---
local configs = {}

function load( filename )
    print("Load " .. filename)
    local file, err = io.open(filename, "r")
    --- no print on assert. wtf?
    if file == nil then
        print(err .. ' in ' .. filename)
        return
    end
    local filedata = file:read("*all")
    local data = assert(loadstring('return ' .. filedata))()
    file:close()    
    for i,j in pairs(data) do
        ctype = string.lower(i)
        if configs[ctype] == nil then
            configs[ctype] = {}
        end
        if j.name == nil then break end
        local name = string.lower(j.name)
        if configs[ctype][name] ~= nil then
            print("Config with name " .. name .. " already loaded. Skipped.\n")
            break
        end
        configs[ctype][name] = j
    end
end

function get( value, subconfig, config )
    ctype = string.lower(config)
    if configs[ctype] == nil then
        print("Config " .. ctype .. " does not exist")  
        return
    end
    ret = configs[ctype][string.lower(subconfig)][value]        
    return ret
end

function getOneFromSeveral( field, config )
    ctype = string.lower(config)
    config = configs[ctype]
    if config == nil then
        print("Config " .. ctype .. " does not exist")
        return
    end
    local t = {}
    for i, j in pairs(config) do
        t[i] = config[i][field]
    end
    local min, max = 0.0, 0.0
    rnd = math.random()
    for i,j in pairs(t) do
        local el =  j / 100.0
        max = max + el
        if ( min < rnd and rnd <= max ) then
            return i
        end
    end
end


---function setDefault( value )    
---    if configs[string.lower(value)] ~= nil then    
---        default = string.lower(value)
---    end
---end

