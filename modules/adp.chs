AdpComps : Elem {
    # "DES adapter module";
    About : Content { = "Agents DES adaptor module"; }
    AdpBase : Syst {
        # "Content AgentUri accepted";
        InpMagUri : CpStateInp;
        # "Link to the node owning managed agents";
        MagOwnerLink : Link;
    }
    NodeAdp : AdpBase
    {
        # "Add content AgentUri to set managed agent URI";
        About : Content { = "MNode DES adaptor"; }
        AdpAgent : AMnodeAdp;
        CompsCount : CpStateOutp;
        CompNames : CpStateOutp;
        Owner : CpStateOutp;
    }
    ElemAdp : AdpBase
    {
        About : Content { = "MElem DES adaptor"; }
        AdpAgent : AMelemAdp;
        InpMut : CpStateInp;
    }
}
