let toQueryObj = (~fetchPolicy=?, result): ApolloClient.queryObj => {
  "query": ApolloClient.gql(. result##query),
  "variables": result##variables,
  "fetchPolicy": fetchPolicy->Js.Nullable.fromOption,
};

let toReadQueryOptions = result => {
  "query": ApolloClient.gql(. result##query),
  "variables": Js.Nullable.fromOption(Some(result##variables)),
};
