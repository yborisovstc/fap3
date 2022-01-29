AdpComps : Elem {
    # "DES adapter module";
    About : Content { = "Agents DES adaptor module"; }
    AdpBase : Syst {
        # "Content AgentUri - Managed agent URI ()";
        # "Content SelfAsMagBase - set self as managed agent base";
        InpMagUri : CpStateInp;
        # "Link to the node owning managed agents";
        MagOwnerLink : Link;
        # "CP provided managed agent owning base";
        InpMagBase : CpStateMnodeInp;
    }
    NodeAdp : AdpBase
    {
        # "Add content AgentUri to set managed agent URI";
        About : Content { = "MNode DES adaptor"; }
        AdpAgent : AMnodeAdp;
        CompsCount : CpStateOutp;
        CompNames : CpStateOutp;
        Owner : CpStateOutp;
        InpMut : CpStateInp;
    }
    ElemAdp : AdpBase
    {
        About : Content { = "MElem DES adaptor"; }
        AdpAgent : AMelemAdp;
        InpMut : CpStateInp;
    }
}
