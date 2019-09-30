module type Config = {
  let query: string;
  type t;
  let parse: Js.Json.t => t;
};

module Make = (Config: Config) => {
  [@bs.module] external gql: ReasonApolloTypes.gql = "graphql-tag";

  type error = {. "message": string};

  [@bs.deriving abstract]
  type options = {
    [@bs.optional]
    fetchPolicy: string,
    [@bs.optional]
    notifyOnNetworkStatusChange: bool,
    [@bs.optional]
    variables: Js.Json.t,
    [@bs.optional]
    pollInterval: int,
    [@bs.optional]
    client: ApolloClient.generatedApolloClient,
  };

  [@bs.module "react-apollo-hooks"]
  external useQuery:
    (ReasonApolloTypes.queryString, Js.Nullable.t(options)) =>
    {
      .
      "data": Js.Nullable.t(Js.Json.t),
      "loading": bool,
      "error": Js.Nullable.t(error),
      "networkStatus": Js.Undefined.t(int),
      "fetchMore":
        {
          .
          "variables": Js.Json.t,
          "updateQuery": ReasonApolloQuery.updateQueryT,
        } =>
        Js.Promise.t(unit),
    } =
    "useQuery";

  type variant =
    | Data(Config.t)
    | Error(error)
    | Loading
    | NoData;

  type result = {
    data: option(Config.t),
    loading: bool,
    error: option(error),
    networkStatus: option(int),
    fetchMore:
      {
        .
        "variables": Js.Json.t,
        "updateQuery": ReasonApolloQuery.updateQueryT,
      } =>
      Js.Promise.t(unit),
  };

  let use = (~options=?, ()) => {
    let jsResult =
      useQuery(gql(. Config.query), Js.Nullable.fromOption(options));

    let result = {
      networkStatus: jsResult##networkStatus->Js.Undefined.toOption,
      fetchMore: jsResult##fetchMore,
      data:
        jsResult##data
        ->Js.Nullable.toOption
        ->Belt.Option.flatMap(data =>
            switch (Config.parse(data)) {
            | parsedData => Some(parsedData)
            | exception _ => None
            }
          ),
      loading: jsResult##loading,
      error: jsResult##error->Js.Nullable.toOption,
    };

    (
      switch (result) {
      | {loading: true} => Loading
      | {error: Some(error)} => Error(error)
      | {data: Some(data)} => Data(data)
      | _ => NoData
      },
      result,
    );
  };
};
