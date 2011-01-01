#!/usr/bin/env lua 
-- This file is part of libflukso.
--
-- (c) Mathias Dalheimer <md@gonium.net>, 2010
--
-- libflukso is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
-- 
-- libflukso is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with libflukso. If not, see <http://www.gnu.org/licenses/>.

-- Check for the configuration file - everything else doesn't make sense
-- without one.

if arg[2] == nil then
  print("Please specify a configuration file, i.e. " .. arg[0] .. " config.lua");
  os.exit(-1);
end
dofile(arg[1])

--todo: detect environment. if on chumby, adjust the module paths,
--on all other platforms, require the luarocks loader.
f=io.open("/bin/chumbyflash", "r");
if f == nil then
  -- this is not a chumby - assume a working luarocks installation.
  is_chumby=false
  require 'luarocks.loader'
else
  -- this is a chumby. set the path manually.
  is_chumby=true
  package.path="/mnt/storage/usr/share/lua/5.1//?.lua;/mnt/storage/usr/share/lua/5.1//?/init.lua;/root/.luarocks/share/lua/5.1//?.lua;/root/.luarocks/share/lua/5.1//?/init.lua;/mnt/storage/usr/share/lua/5.1//?.lua;/mnt/storage/usr/share/lua/5.1//?/init.lua;/mnt/storage/usr/share/lua/5.1//?.lua;/mnt/storage/usr/share/lua/5.1//?/init.lua;./?.lua;/usr/local/share/lua/5.1/?.lua;/usr/local/share/lua/5.1/?/init.lua;/usr/local/lib/lua/5.1/?.lua;/usr/local/lib/lua/5.1/?/init.lua"
  package.cpath="/mnt/storage/usr/lib/lua/5.1//?.so;/root/.luarocks/lib/lua/5.1//?.so;./?.so;/usr/local/lib/lua/5.1/?.so;/usr/local/lib/lua/5.1/loadall.so"
end

require 'posix'
ox=posix
if is_chumby then
  ox.openlog('chumbydaemon')
end
function log(line)
  if is_chumby then
    ox.syslog(30, line)
  else
    print(line)
  end
end

log("starting C8H10N4O2 data aggregator daemon")

local command = {
  last_update = {
        cmd = config.BINPATH .. config.CMD .. 
              " -l " .. config.IP .. 
              " -s " .. config.SENSOR ..
              " -n " .. config.DATADIR .. "/last_reading" ..
              " -f chumby-current -o file",
        interval = 1
      },
}
ox.mkdir(config.DATADIR);

function ticker()
  return coroutine.create (function ()
    while true do
      ox.sleep(1)
      coroutine.yield()
    end
  end
  )
end

function timeactor(tupstream, command)
  return coroutine.create(function()
    local i=command.interval;
    log("using interval " .. i .. " for command " .. command.cmd)
    while true do
      if (i > 0) then
        i = i - 1;
      else
        retval = os.execute(command.cmd)
        if not retval==0 then
          log("Error while running " .. command .. 
              " -> return code " .. retval)
        end
        i=command.interval;
      end
      coroutine.resume(tupstream)
      coroutine.yield()
    end
  end
  )
end

function controller(cupstream)
  return coroutine.create(function()
    while true do
      coroutine.resume(cupstream)
    end
  end
  )
end

local chain = 
controller(
  timeactor(
    timeactor(ticker(), command.last_reading)
  , command.last_minute)
)

coroutine.resume(chain);
