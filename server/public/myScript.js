var originalSystemPosition = {x:0,y:1000,a:500};
var oldSystemPosition = {x:0,y:1000,a:500};

$("#X_Slider").rangeslider({
  polyfill: false,
  onSlideEnd: function(position, value){
    if($("input[name=option]:checked").val() == "manual"){
      addInstruction();
      sendInstructions();
    }
  }
});

$("#Y_Slider").rangeslider({
  polyfill: false,
  onSlideEnd: function(position, value){
    if($("input[name=option]:checked").val() == "manual"){
      addInstruction();
      sendInstructions();
    }
  }
});

$("#rotationSlider").rangeslider({
  polyfill: false,
  onSlideEnd: function(position, value){
    if($("input[name=option]:checked").val() == "manual"){
      addInstruction();
      sendInstructions();
    }
  }
});

function resetPosition(){
  var instructionToAdd = {"X_axisMovement": -1000, "Y_axisMovement": -1000};
  $("#instructionsList").append('<li class="list-group-item">' + JSON.stringify(instructionToAdd) + '</li>');
  sendInstructions();
}

function addInstruction(){
  var instructionToAdd = {"X_axisMovement":$("#X_Slider").val()-oldSystemPosition.x, "A_axisMovement":$("#rotationSlider").val()-oldSystemPosition.a, "Y_axisMovement":$("#Y_Slider").val()-oldSystemPosition.y};
  $("#instructionsList").append('<li class="list-group-item">' + JSON.stringify(instructionToAdd) + '</li>');
  updateOldPosition();
}

function updateOldPosition(){
  oldSystemPosition.x = $("#X_Slider").val();
  oldSystemPosition.y = $("#Y_Slider").val();
  oldSystemPosition.a = $("#rotationSlider").val();
}

function sendInstructions(){
  var allInstructions = $( "ul" ).children();
  var nbrOfInstruction = allInstructions.length;
  if(nbrOfInstruction > 0){
    enableOrDisableButtons(true);
    $("#X_Slider").val(originalSystemPosition.x).change();
    $("#Y_Slider").val(originalSystemPosition.y).change();
    $("#rotationSlider").val(originalSystemPosition.a).change();
    executeInstruction(allInstructions.toArray());
  }
}

function executeInstruction(listOfInstruction){
  if(listOfInstruction.length > 0){
    var liToExecute = $(listOfInstruction[0]);//We need the Jquery version of the element
    listOfInstruction.shift(); //Let's remove the first element
    var jsonInstruction = JSON.parse(liToExecute.text());

    $.post( "/cnc/command", jsonInstruction, function(data) {
      if(!data.success){
        $("#rotationSlider").val(jsonInstruction.A_axisMovement+parseInt($("#rotationSlider").val())).change();
        $("#X_Slider").val(jsonInstruction.X_axisMovement+parseInt($("#X_Slider").val())).change();
        $("#Y_Slider").val(jsonInstruction.Y_axisMovement+parseInt($("#Y_Slider").val())).change();

        liToExecute.remove();
        setTimeout(executeInstruction.bind(null, listOfInstruction),2000);
      }
    });
  }else{//We just finished to read all the instructions we can enable the buttons
    enableOrDisableButtons(false);
    originalSystemPosition.x = $("#X_Slider").val();
    originalSystemPosition.y = $("#Y_Slider").val();
    originalSystemPosition.a = $("#rotationSlider").val();
  }
}

function enableOrDisableButtons(enable){
  $('button').prop('disabled', enable);
  $("#X_Slider").prop("disabled", enable);
  $("#Y_Slider").prop("disabled", enable);
  $("#rotationSlider").prop("disabled", enable);
  $("#X_Slider").rangeslider('update');
  $("#Y_Slider").rangeslider('update');
  $("#rotationSlider").rangeslider('update');
}
