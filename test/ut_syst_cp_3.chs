MyRoot : Elem {
    SS : Syst {
        S1 : Syst {
            Agt : TstAgt;
            Cp1 : ConnPointu { Provided = "MTIf1"; Required = "Ifc2"; }
        }
        Cp2 : ConnPointu { Provided = "Ifc2"; Required = "MTIf1"; }
        Cp2 ~ S1.Cp1;
    }
}
