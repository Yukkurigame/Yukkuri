
Environment = {
	day_length = 240000,
	day_period = 200,
	latitude = 10,
	longitude = 130,
	gtm_delta = 11,
	time = 8,
	day = 340,
}
Environment.__index = Environment

local RADIANS = math.pi / 180

function Environment:setup(params)
	self.ltsm = ( self.longitude - 15.0 * self.gtm_delta )
	self.sun = Lights.create(constants.ltDirectional)
	self.sun:color({1,1,1})
	self.sun:diffuse(2)
	if self.thread == nil then
		self.thread = Thread.newThread(function()
			while true do
				Environment:update()
				Thread.wait(Environment.day_period)
			end
		end, true)
	end
	Thread.resume(self.thread)
end


function Environment:setText(text)
	local widget = GUI:getWidget('time')
	if widget ~= nil then
		widget:WidgetText(text)
	end
end


function Environment:update()
	local dt = self.day_period
	local hours = 24 * dt / self.day_length
	self.time = (self.time + hours) % 24
	local time = self.time
	if time > 22 or time < 2 then
		--[[ if Time < 2 then
			if( !Updated ){
				Day++;
				Updated = true;
			}
		}else{
			if( Updated )
				Updated = false;
		} -- ]]--
		self:setText("Midnight")
	elseif time > 18 then
		self:setText("Evening")
		--[[ if(Time > 20.0){
			if(text)
				text->setText("Twilight");
		}else{
			if(text)
				text->setText("Evening");
		}-- ]]--
	elseif time < 6 then
		self:setText("Night")
		--[[if( Time > 4 ){
			if(text)
				text->setText("Dawn");
		}else{
			if(text)
				text->setText("Night");
		}-- ]]--
	elseif time < 11 then
		self:setText("Morning")
	elseif time > 14 then
		self:setText("Afternoon")
	else
		self:setText("Noon")
	end
	--hours, seconds = math.modf(time)
	--self:setText(string.format("%d:%d", hours, seconds * 60))

	self:update_weather()
end


function Environment:update_weather()
	if self.sun ~= nil then
		self.sun:direction(self:sun_direction())
	end
end


function Environment:sun_direction( )
	-- Realistic, lol: http://pveducation.org/pvcdrom/properties-of-sunlight/suns-position
	local B = (360.0 / 365.0) * ((self.day % 365) - 81.0) * RADIANS;
	local EoT = 9.87 * math.sin(2.0*B) - 7.53 * math.cos(B) - 1.5 * math.sin(B);
	local TC = 4.0 * self.ltsm + EoT;
	local LST = self.time + TC / 60.0;
	local HRA = 15.0 * RADIANS * ( LST - 12.0 );
	local delta = 23.45 * RADIANS * math.sin(B);
	local latsin = math.sin(self.latitude * RADIANS)
	local latcos = math.cos(self.latitude * RADIANS)

	local Elevation = math.asin(math.sin(delta) * latsin + math.cos(delta) * latcos * math.cos(HRA));
	local Azimuth = math.acos(
			( math.sin(delta) * latcos - math.cos(delta) * latsin * math.cos(HRA) ) /
				math.cos(Elevation));
	if HRA > 0 then
		Azimuth = 2 * math.pi - Azimuth;
	end
	local ret = {
		math.cos(Elevation) * math.sin(Azimuth),
		math.sin(Elevation) * math.sin(Azimuth),
		-math.sin(Elevation)
	}
	return ret
end
