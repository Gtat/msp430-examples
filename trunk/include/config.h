struct parameter_t parameters =
  {
    .process  = NULL,
    .processing_option = NULL,
    .rate     =
    {
      .taccr = 0xF424,
    },
    .voltages =
    {
      [0] = 
      {
        .range = 0,
        .data  = 0x01,
      },
      [1] = 
      {
        .range = 0,
        .data  = 0x23,
      },
      [2] = 
      {
        .range = 0,
        .data  = 0x45,
      },
      [3] = 
      {
        .range = 0,
        .data  = 0x67,
      },
      [4] = 
      {
        .range = 0,
        .data  = 0x89,
      },
      [5] = 
        {
          .range = 0,
          .data  = 0xAB,
        },
    },
  };
