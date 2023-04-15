#version 330 core

in vec3 ourNormal;
in vec3 ourColor;
in vec3 FragPos;

uniform vec3 luzColor;
uniform vec3 luzPos;
uniform vec3 vistaPos;

void main(){
	//direccion de la luz hacia abajo
	vec3 luzDir = vec3(0, 0, -1.0f);

	float ambientI = 0.5;
	vec3 ambiente = ambientI * luzColor;

	vec3 ld = normalize(-luzPos);
	ld = luzDir;
	vec3 fd = normalize(vec3(FragPos-luzPos));
	if(acos(dot(ld,fd)) < radians(15)){
		//difusa
		vec3 norm = normalize(ourNormal);
		vec3 luzDir = normalize(luzPos - FragPos);
		float diff = max(dot(norm, luzDir), 0);
		vec3 difusa = diff * luzColor;

		//especular
		float fuerzaEspecular = 1.0;
		vec3 vistaDir = normalize(vistaPos - FragPos);
		vec3 reflexDir = reflect(-luzDir, norm);
		float espec = pow(max(dot(vistaDir, reflexDir), 0.0), 128);
		vec3 especular = fuerzaEspecular * espec * luzColor;

		//resultado
		vec3 resultado = (ambiente + difusa + especular) * ourColor;

		gl_FragColor = vec4 (resultado, 1.0f);
	}else{
		vec3 resultado = ambiente * ourColor;
		gl_FragColor = vec4 (resultado, 1.0f);
	}
}