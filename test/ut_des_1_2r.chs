MyRoot : Elem {
    Launcher : DesLauncher {
        St1 : State (
            _@ <  {
                = "SI 0"
            }
            Inp ~ Tradd : TrAdd2Var (
                Inp ~ St1
                Inp2 ~ : SI_1
            )
        )
    }
}
