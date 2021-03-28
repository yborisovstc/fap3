MyRoot : Elem {
    SS : Syst {
        S1 : Syst {
            Agt : TstAgt;
            Agt2 : TstAgt;
            Cp1 : ConnPointu { Provided = "MTIf1"; Required = "Ifc2"; }
            Ext1 : Extd {
                Int : ConnPointu { Provided = "Ifc2"; Required = "MTIf1"; }
            }
            Ext1.Int ~ Cp1;
        }
        Cp2 : ConnPointu { Provided = "Ifc2"; Required = "MTIf1"; }
        Cp2 ~ S1.Ext1;
    }
}
