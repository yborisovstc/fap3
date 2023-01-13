Root : Elem {
    # "Test of SDO #2, edges"
    Launcher : DesLauncher {
        Explorable = "y"
        # "Components to be checked"
        V1 : Vert
        V2 : Vert
        V3 : Vert
        V4 : Vertu {
            V4_1 : Vertu
        }
        V5 : Vertu {
            Explorable = "y"
            V5_1 : Vertu
        }
        V6 : Vertu
        V1 ~ V2
        V1 ~ V4
        V5 ~ V4
        V5 ~ V3
        V4.V4_1 ~ V6
        # "Checking existence of component"
        Dbg_DcoEdges : State @  {
            _@ <  {
                Debug.LogLevel = "Dbg"
                = "VPDU _INV"
            }
            Inp ~ DcoComp : SdoEdges @  {
                _@ < Debug.LogLevel = "Dbg"
            }
        }
    }
}
