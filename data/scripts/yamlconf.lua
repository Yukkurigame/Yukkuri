
---
local configs = {}
local default = nil

function load( filename )
    if package.loaded.yaml == nil then require 'yaml' end
    print("Load " .. filename)
    local file, err = io.open(filename, "r")
    --- no print on assert. wtf?
    if file == nil then
        print(err .. ' in ' .. filename)
        return
    end
    local filedata = file:read("*all")    
    local yamldata = yaml.load(filedata)
    file:close()    
    for i in ipairs(yamldata) do
        if yamldata[i].name == nil then break end
        local name = string.lower(yamldata[i].name)
        if configs[name] ~= nil then
            print("Config with name" .. name .. " already loaded. Skipped.\n")
            break
        end
        if default == nil then
            default = name
        end
        configs[name] = yamldata[i]
    end    
end

function get( value, config )        
    if config == nil then
        config = default
    end
    return configs[string.lower(config)][value]
end

function setDefault( value )    
    if configs[string.lower(value)] ~= nil then
        default = string.lower(value)
    end
end

