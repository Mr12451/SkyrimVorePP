;/ Decompiled by Champollion V1.0.1
Source   : PF_DDAP_FollowerEatenDismissFragment.psc
Modified : 2024-01-29 08:54:53
Compiled : 2024-01-29 08:54:54
User     : Josh
Computer : DESKTOP-4SIUDR1
/;
scriptName PF_DDAP_FollowerEatenDismissFragment extends Package hidden

;-- Properties --------------------------------------
quest property pDialogueFollower auto

;-- Variables ---------------------------------------

;-- Functions ---------------------------------------

; Skipped compiler generated GotoState

function Fragment_1(Actor akActor)

	(pDialogueFollower as dialoguefollowerscript).DismissFollower(0, 0)
endFunction

; Skipped compiler generated GetState

function Fragment_0(Actor akActor)

	(pDialogueFollower as dialoguefollowerscript).DismissFollower(0, 0)
endFunction
