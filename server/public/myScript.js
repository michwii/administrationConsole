$("#X_Slider").rangeslider({
  polyfill: false,
  onSlide: function(position, value) {
    updateAxesValueLabel();
  },
  onSlideEnd: function(position, value){
    if($("input[name=modeAutoManual]:checked").val() == "manual"){
      addInstruction();
      sendInstructions();
    }
  }
});

$("#Y_Slider").rangeslider({
  polyfill: false,
  onSlide: function(position, value) {
    updateAxesValueLabel();
  },
  onSlideEnd: function(position, value){
    if($("input[name=modeAutoManual]:checked").val() == "manual"){
      addInstruction();
      sendInstructions();
    }
  }
});

$("#rotationSlider").rangeslider({
  polyfill: false,
  onSlideEnd: function(position, value){
    if($("input[name=modeAutoManual]:checked").val() == "manual"){
      addInstruction();
      sendInstructions();
    }
  },
  onSlide: function(position, value) {
    updateAxesValueLabel();
    if($("input[name=modeLockUnlock]:checked").val() == "lock"){

      var angleDifference = getAngularPosition(value - oldSystemPosition.a) ;
      var offsetX = Math.round(getXOffset(angleDifference));
      var sliderValue = parseInt(oldSystemPosition.x);
      $("#X_Slider").val(sliderValue + offsetX).change();
    }
  }
});

$("#gripperSlider").rangeslider({
  polyfill: false,
  onSlideEnd: function(position, value){
    if($("input[name=modeAutoManual]:checked").val() == "manual"){
      addInstruction();
      sendInstructions();
    }
  }
});

updateAxesValueLabel();

var originalSystemPosition = {x:$("#X_Slider").val(),y:$("#Y_Slider").val(),a:$("#rotationSlider").val(), b:$("input[name=B_Slider]:checked").val(), gripper:$("#gripperSlider").val()};
var oldSystemPosition = {x:$("#X_Slider").val(),y:$("#Y_Slider").val(),a:$("#rotationSlider").val(), b:$("input[name=B_Slider]:checked").val(), gripper:$("#gripperSlider").val()};

function updateAxesValueLabel(){
  $("#X_labelValue").text($("#X_Slider").val());
  $("#Y_labelValue").text($("#Y_Slider").val());
  $("#rotationLabelValue").text($("#rotationSlider").val());
}

function getAngularPosition(position){
  return (position * 180) / 160000;
}

function getXOffset(angular){
  var distanceInCm = 24 * 2 * Math.PI * (((angular * 100)/360)/100) ;
  return (15000 * distanceInCm) /35 ;
}

function resetPosition(){
  var instructionToAdd = {"X_axisMovement": -2000000, "Y_axisMovement": 2000000, gripperMovement: 20, B_axisMovement: 95};
  $("#instructionsList").append('<li class="list-group-item">' + JSON.stringify(instructionToAdd) + '</li>');
  sendInstructions();
}

function addInstruction(){
  var instructionToAdd = {
    "X_axisMovement":$("#X_Slider").val()-oldSystemPosition.x,
    "A_axisMovement":$("#rotationSlider").val()-oldSystemPosition.a,
    "Y_axisMovement":$("#Y_Slider").val()-oldSystemPosition.y,
    "B_axisMovement":$("input[name=B_Slider]:checked").val(),
    "gripperMovement":$("#gripperSlider").val()
  };
  $("#instructionsList").append('<li class="list-group-item">' + JSON.stringify(instructionToAdd) + '</li>');
  updateOldPosition();
}

function updateOldPosition(){
  oldSystemPosition.x = $("#X_Slider").val();
  oldSystemPosition.y = $("#Y_Slider").val();
  oldSystemPosition.a = $("#rotationSlider").val();
  oldSystemPosition.b = $("input[name=B_Slider]:checked").val();
  oldSystemPosition.gripper = $("#gripperSlider").val();
}

function sendInstructions(){
  var allInstructions = $( "ul" ).children();
  var nbrOfInstruction = allInstructions.length;
  if(nbrOfInstruction > 0){
    enableOrDisableButtons(true);
    $("#X_Slider").val(originalSystemPosition.x).change();
    $("#Y_Slider").val(originalSystemPosition.y).change();
    $("#rotationSlider").val(originalSystemPosition.a).change();
    $("input[name=B_Slider][value='"+ originalSystemPosition.b +"']").prop("checked",true);
    $("#gripperSlider").val(originalSystemPosition.gripper).change();
    executeInstruction(allInstructions.toArray());
  }
}

function executeInstruction(listOfInstruction){
  if(listOfInstruction.length > 0){
    var liToExecute = $(listOfInstruction[0]);//We need the Jquery version of the element
    listOfInstruction.shift(); //Let's remove the first element
    var jsonInstruction = JSON.parse(liToExecute.text());

    $.post( "/cnc/command", jsonInstruction, function(data) {
      if(data.success){
        $("#rotationSlider").val(jsonInstruction.A_axisMovement+parseInt($("#rotationSlider").val())).change();
        $("#X_Slider").val(jsonInstruction.X_axisMovement+parseInt($("#X_Slider").val())).change();
        $("#Y_Slider").val(jsonInstruction.Y_axisMovement+parseInt($("#Y_Slider").val())).change();
        $("input[name=B_Slider][value='"+ jsonInstruction.B_axisMovement +"']").prop("checked",true);
        $("#gripperSlider").val(jsonInstruction.gripperMovement).change();
        liToExecute.remove();
        setTimeout(executeInstruction.bind(null, listOfInstruction),8000);
      }
    });
  }else{//We just finished to read all the instructions we can enable the buttons
    enableOrDisableButtons(false);
    originalSystemPosition.x = $("#X_Slider").val();
    originalSystemPosition.y = $("#Y_Slider").val();
    originalSystemPosition.a = $("#rotationSlider").val();
    originalSystemPosition.b = $("input[name=B_Slider]:checked").val();
    originalSystemPosition.gripper = $("#gripperSlider").val();
  }
}

function enableOrDisableButtons(enable){
  $('button').prop('disabled', enable);
  $("#X_Slider").prop("disabled", enable);
  $("#Y_Slider").prop("disabled", enable);
  $("#rotationSlider").prop("disabled", enable);
  $("#gripperSlider").prop("disabled", enable);
  $("input[name=B_Slider]").prop("disabled", enable);

  $("#X_Slider").rangeslider('update');
  $("#Y_Slider").rangeslider('update');
  $("#rotationSlider").rangeslider('update');
  $("#gripperSlider").rangeslider('update');
}
