
function sampleDropObject(player, entity)

   outputString("GLOBAL EVENT: triggered by dropping an object...\n")

   -- first value decides if we should continue executing event handlers
   -- second value decides if we should execute the action that triggered event
   return true, true

end

