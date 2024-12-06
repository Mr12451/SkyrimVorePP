scriptName SVPP_API hidden

Function PreyDialogue() global native
Function PredDialogue() global native
Function ActorDialogue(Actor target) global native

Function Swallow(Actor pred, ObjectReference prey, Float locus, Float digType, Bool fullSwallow) global native
Function Regurgitate(Actor pred, ObjectReference prey) global native
Function RegurgitateAll(Actor pred, Float locus) global native

Function Consent(Actor pred, ObjectReference prey, Bool willing, Bool lethal) global native
Function DisableEscape(ObjectReference prey) global native
Function SwitchTo(ObjectReference prey, Float digType) global native
Function SwitchPredTo(Actor pred, Float digType, Bool noEscape) global native
Function SetPredControl(Bool allowControl) global native
Function KillPrey(Actor prey) global native
Function PlanReformation(Actor prey) global native

Function Strip(Actor target) global native
float function GetDefaultLocus() global native
float function GetRandomLocus(Actor target) global native